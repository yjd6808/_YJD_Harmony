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
void IOCP::SetListener(const jc::SharedPtr<IOCPTaskListener>& _pListener)
{
	if (state_ == State::Destroyed)
	{
		jc_assert_msg(false, "Destroyed 상태의 IOCP는 리스너를 등록할 수 없습니다.");
		return;
	}

	if (pListener_ != nullptr)
	{
		jc_assert_msg(false, "이미 리스너가 등록되어 있습니다.");
		return;
	}

	pListener_ = _pListener;
}

//////////////////////////////////////////////////////////////////////////////////////////
int IOCP::PollTasks()
{
	// 단일 쓰레드에서만 호출되도록 해야함. (일반적으로 메인쓰레드)

	if (pListener_ == nullptr)
	{
		_NetLogWarn_("IOCP에 리스너가 등록되지 않았습니다. PollTasks를 호출하기 전에 SetListener로 리스너를 등록해주세요.");
		return 0;
	}

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

	auto pListener = pListener_.GetPtr();
	int processedTaskCount = 0;
	for (int i = 0; i < cachedTaskList_.Size(); ++i)
	{
		IOCPTaskAbstractPtr& pTask = cachedTaskList_[i];
		pListener->OnTaskCompleted(pTask.GetPtr());
		++processedTaskCount;
	}
	return processedTaskCount;
}
