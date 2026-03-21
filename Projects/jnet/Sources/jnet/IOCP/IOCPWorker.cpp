/*
 * 작성자 : 윤정도
 */

#include "jnet/IOCP/IOCPWorker.h"
#include "jnet/IOCP/IOCPPostOrder.h"
#include "jnet/IOCPOverlapped/IOCPOverlapped.h"

#include "jc/Primitives/RefCountObjectPtr.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
IOCPWorker::IOCPWorker(IOCP* _pIocp)
: Worker()
, iocp_(_pIocp)
, taskQueue_(dbg_new jc::ListQueue<IOCPTaskAbstractPtr>())
, swapTaskQueue_(dbg_new jc::ListQueue<IOCPTaskAbstractPtr>())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPWorker::~IOCPWorker()
{
	JC_DELETE_SAFE(taskQueue_);
	JC_DELETE_SAFE(swapTaskQueue_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPWorker::Run(void* _pParam)
{
	(void)_pParam;

	// std::thread 멤버 함수로 실행하는 법
	// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function
	//  ==> 내가 구현한 쓰레드로 변경

	state_ = State::eRunning;
	thread_.Start([this](void* _pThreadParam)
	{
		WorkerThread(_pThreadParam);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPWorker::JoinWait(WaitHandle* _pWaitHandle)
{
	IOCPPostOrder* pPostOrder = dbg_new IOCPPostOrder;
	pPostOrder->waitHandle_ = _pWaitHandle;
	pPostOrder->order_ = IOCP_POST_ORDER_TERMINATE;
	const ULONG_PTR completionKey = reinterpret_cast<ULONG_PTR>(pPostOrder);

	// 어느 쓰레드가 꺠어날지 모르기 때문에 여기서 join을 수행하면 안됨
	if (!iocp_->Post(0, completionKey, nullptr))
	{
		_NetLogWarn_("IOCPWorker::Pause() Failed");
		pPostOrder->Release();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPWorker::Join()
{
	thread_.Join();
	state_ = State::eJoined;
}

// 문재점!
// IOCPAcceptOverlapeed 동적할당을 해제 해주도록 하는 로직이 필요하다.
// 지금 Join() 함수 호출 후 PostQueue로 쓰레드에 신호를 주면 반복문을 나와버려서 남은 오버랩이 처리 안되서 해제 불가능하게된다.
// --> 서버 종료시를 제외하고는 Join() 함수 호출을 하면안됨
//////////////////////////////////////////////////////////////////////////////////////////

void IOCPWorker::WorkerThread(void* _pParam)
{
	(void)_pParam;
	const _u32l BATCH_SIZE = iocp_->GetBatchSize();
	_NetLogDebug_("IOCPWorker 쓰레드가 실행되었습니다. (id: %d, batch size: %u)", Thread::GetThreadId(), BATCH_SIZE);
	
	OVERLAPPED_ENTRY* entries = dbg_new OVERLAPPED_ENTRY[BATCH_SIZE];
	_u32l numEntriesRemoved = 0;

	for (;;)
	{
		BOOL result = FALSE;

		if (BATCH_SIZE >= 2)
		{
			result = iocp_->GetStatusEx(entries, BATCH_SIZE, &numEntriesRemoved);

			if (!result || numEntriesRemoved == 0)
			{
				continue;
			}
		}
		else
		{
			_u32l numberOfBytesTransferred = 0;
			ULONG_PTR completionKey = NULL;
			LPOVERLAPPED pOverlapped = nullptr;

			result = iocp_->GetStatus(&numberOfBytesTransferred, &completionKey, &pOverlapped);

			entries[0].dwNumberOfBytesTransferred = numberOfBytesTransferred;
			entries[0].lpCompletionKey = completionKey;
			entries[0].lpOverlapped = pOverlapped;
			numEntriesRemoved = 1;
		}

		for (_u32l i = 0; i < numEntriesRemoved; ++i)
		{
			const OVERLAPPED_ENTRY& entry = entries[i];
			IOCPPostOrder* pPostOrder = reinterpret_cast<IOCPPostOrder*>(entry.lpCompletionKey);
			IOCPOverlapped* pIocpOverlapped = static_cast<IOCPOverlapped*>(entry.lpOverlapped);

			if (pIocpOverlapped)
			{
				JC_REF_COUNT_GUARD(pIocpOverlapped, false);
				pIocpOverlapped->Process(TRUE, entry.dwNumberOfBytesTransferred, pPostOrder, this);
				continue;
			}

			if (entry.dwNumberOfBytesTransferred == 0 && pPostOrder)
			{
				JC_REF_COUNT_GUARD(pPostOrder, false);

				switch (pPostOrder->Process(this))
				{
				case IOCP_POST_ORDER_TERMINATE:
					goto THREAD_END;
				case IOCP_POST_ORDER_ERROR:
					jc_assert_msg(false, "오류");
					break;
				default:
					jc_assert_msg(false, "이상한 타입의 포스트 오더입니다.");
					break;
				}
			}
		}

		numEntriesRemoved = 0;
	}

THREAD_END:
	delete[] entries;
	_NetLogDebug_("%s IOCPWorker 쓰레드가 종료되었습니다. (%d)", iocp_->GetName().Source(), Thread::GetThreadId());
	state_ = State::eJoinWait;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPWorker::EnqueueTask(const IOCPTaskAbstractPtr& _pTask)
{
	JC_LOCK_GUARD(taskQueueLock_);
	taskQueue_->Enqueue(_pTask);
	hasTask_.Store(true);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOCPWorker::PopAllTasksWithSwap(jc::Vector<IOCPTaskAbstractPtr>& _v)
{
	if (!hasTask_)
		return;
	// 여러쓰레드에서 호출 금지. 무조건 하나의 쓰레드만 이 함수를 호출해야한다.
	jc::ListQueue<IOCPTaskAbstractPtr>* pQueue = nullptr;
	{
		JC_LOCK_GUARD(taskQueueLock_);
		hasTask_.Store(false);
		pQueue = taskQueue_;
		taskQueue_ = swapTaskQueue_;
		swapTaskQueue_ = pQueue;
	}

	if (pQueue->Size() > 0)
	{
		while (!pQueue->IsEmpty())
		{
			_v.PushBack(pQueue->Front());
			pQueue->Dequeue();
		}
	}
}
