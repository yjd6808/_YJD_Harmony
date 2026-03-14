/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/IOCP/IOCP.h>
#include <jnet/WorkerGroup.h>
#include <jnet/IOCP/IOCPWorker.h>

#include <jc/Threading/Pulser.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
IOCP::IOCP(int _threadCount)
: state_(State::Initialized)
, iocpHandle_(INVALID_HANDLE_VALUE)
, threadCount_(_threadCount)
, workerManager_(nullptr)
, name_(0)
{
	if ((iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, static_cast<_u32l>(_threadCount))) == INVALID_HANDLE_VALUE)
	{
		jc_assert_msg(false, "IOCP를 생성하는데 실패했습니다.");
	}

	threadCount_ = _threadCount;
	workerManager_ = WorkerGroup::Create<IOCPWorker>(_threadCount, this);
	state_ = State::Initialized;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCP::~IOCP()
{
	if (state_ == State::Running)
	{
		jc_assert_msg(false, "먼저 조인을 해주세요");
	}

	if (state_ != State::Destroyed)
	{
		Destroy();
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::Destroy()
{
	if (state_ == State::Destroyed)
	{
		return;
	}

	if (iocpHandle_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(iocpHandle_);
		iocpHandle_ = INVALID_HANDLE_VALUE;
	}

	JC_DELETE_SAFE(workerManager_);
	state_ = State::Destroyed;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::Run()
{
	if (state_ != State::Initialized)
	{
		jc_assert_msg(false, "Initialized 상태의 IOCP만 Run할 수 있습니다.");
		return;
	}

	{
		JC_LOCK_GUARD(workerManagerLock_);
		workerManager_->Run();
	}
	state_ = State::Running;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::Join()
{
	if (state_ != State::Running)
	{
		jc_assert_msg(false, "Running 상태의 IOCP만 Join 할 수 있습니다.");
		return;
	}

	// 펜딩카운트가 0이 될때까지 기다린다.
	WaitForZeroPending();

	{
		JC_LOCK_GUARD(workerManagerLock_);
		workerManager_->Join();
	}
	state_ = State::Joined;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::WaitForZeroPending()
{
	Pulser pulser(10, Pulser::eSliceCycle);
	TimeCounter logCounter(TimeCounterAttribute::TimeOverReset);

	pulser.Start();

	while (true)
	{
		TimeSpan elapsed = pulser.Wait();
		logCounter.Elapsed += elapsed;

		int pending = pendingOverlappedCount_;
		if (pending <= 0)
		{
			jc_assert_msg(pending >= 0, "멍미 펜딩 카운트가 움수 인뎁쇼 (%d)", pending);
			break;
		}

		if (logCounter.ElapsedSeconds(1))
		{
			_NetLogDebug_("IOCP::WaitForZeroPending - pendingOverlappedCount_: %d", pending);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Vector<_u32> IOCP::GetWorkThreadIdList()
{
	JC_LOCK_GUARD(workerManagerLock_);
	jc::Vector<_u32> threadIdList(workerManager_->workers_.Size());
	for (int index = 0; index < workerManager_->workers_.Size(); ++index)
	{
		threadIdList.PushBack(workerManager_->workers_[index]->GetThreadId());
	}
	return threadIdList;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOCP::Connect(_whandle _handle, ULONG_PTR _completionKey) const
{
	// @참고: https://learn.microsoft.com/en-us/windows/win32/fileio/createiocompletionport
	// 연결시 NumberOfConcurrentThreads 파라미터 값은 무시된다.
	// ExistingCompletionPort를 NULL이 아닌 유효한 IOCP 핸들을 전달한 후 성공적으로 연결되면 ExistingCompletionPort 파라미터의 핸들과 같은 핸들을 반환한다.
	return CreateIoCompletionPort(_handle, iocpHandle_, _completionKey, threadCount_) == iocpHandle_;
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL IOCP::GetStatus(_u32l* _pNumberOfBytesTransffered, PULONG_PTR _pCompletionKey, LPOVERLAPPED* _ppOverlapped) const
{
	return GetQueuedCompletionStatus(iocpHandle_, _pNumberOfBytesTransffered, _pCompletionKey, _ppOverlapped, INFINITE);
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL IOCP::Post(_u32l _numberOfBytesTransferred, ULONG_PTR _completionKey, LPOVERLAPPED _pOverlapped) const
{
	return PostQueuedCompletionStatus(iocpHandle_, _numberOfBytesTransferred, _completionKey, _pOverlapped);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::SetName(const jc::String& _name)
{
	name_ = _name;
}

//////////////////////////////////////////////////////////////////////////////////////////
int IOCP::PollTasks()
{
	if (pollingMode_ == false)
	{
		jc_assert_msg(false, "폴링 모드가 아닙니다. PollTasks를 호출하기 전에 SetPollingMode(true)로 폴링 모드로 설정해주세요.");
		return 0;
	}

	// 단일 쓰레드에서만 호출되도록 해야함. (일반적으로 메인쓰레드)
	cachedTaskList_.Clear();
	{
		JC_LOCK_GUARD(workerManagerLock_);
		auto& workers = workerManager_->workers_;
		for (int i = 0; i < workers.Size(); ++i)
		{
			IOCPWorker* pWorker = static_cast<IOCPWorker*>(workers[i]);
			if (!pWorker->HasTask())
				continue;

			pWorker->PopAllTasksWithSwap(cachedTaskList_);
		}
	}

	int processedTaskCount = 0;
	if (fnTaskCompleted_)
	{
		for (int i = 0; i < cachedTaskList_.Size(); ++i)
		{
			IOCPTaskAbstractPtr& pTask = cachedTaskList_[i];
			fnTaskCompleted_(pTask.GetPtr());
			++processedTaskCount;
		}
	}
	return processedTaskCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCP::OnTaskCompleted(IOCPTaskAbstract* _pTask)
{
	// polling 모드가 아닌 경우에 IOCPOverlappedTask에서 호출됨.
	if (pollingMode_ == false)
	{
		if (fnTaskCompleted_)
		{
			fnTaskCompleted_(_pTask);
		}
	}
}
	
