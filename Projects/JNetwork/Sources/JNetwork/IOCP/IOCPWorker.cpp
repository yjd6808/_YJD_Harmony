/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/IOCP/IOCPWorker.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

using namespace JCore;

NS_JNET_BEGIN

IOCPWorker::IOCPWorker(IOCP* iocp)  : Worker()
                                    ,  m_pIocp(iocp) {
}

IOCPWorker::~IOCPWorker() = default;

void IOCPWorker::Run(void* param) {
	// std::thread 멤버 함수로 실행하는 법
	// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function
	//  ==> 내가 구현한 쓰레드로 변경

	m_eState = State::Running;
	m_Thread = Thread([this](void* param) { WorkerThread(param); });
}

void IOCPWorker::JoinWait(WinHandle waitHandle) {
	IOCPPostOrder* pPostOrder = dbg_new IOCPPostOrder{ IOCP_POST_ORDER_TERMINATE, waitHandle };
	const ULONG_PTR completionKey = (ULONG_PTR)pPostOrder;

	if (m_pIocp->Post(0, completionKey, nullptr) == FALSE) {  // 어느 쓰레드가 꺠어날지 모르기 때문에 여기서 join을 수행하면 안됨
		DebugAssertMsg(false, "IOCPWorker::Pause() Failed");
		pPostOrder->Release();
	}
}

void IOCPWorker::Join() {
	m_Thread.Join();
	m_eState = State::Joined;
}


// 문재점!
// IOCPAcceptOverlapeed 동적할당을 해제 해주도록 하는 로직이 필요하다.
// 지금 Join() 함수 호출 후 PostQueue로 쓰레드에 신호를 주면 반복문을 나와버려서 남은 오버랩이 처리 안되서 해제 불가능하게된다.
// --> 서버 종료시를 제외하고는 Join() 함수 호출을 하면안됨

void IOCPWorker::WorkerThread(void* param) {
	NetLog("IOCPWorker 쓰레드가 실행되었습니다. (%d)\n", Thread::GetThreadId());

	for (;;) {
		Int32UL numberOfBytesTransffered;
		ULONG_PTR completionKey = NULL;
		OVERLAPPED* pOverlapped = nullptr;

		const BOOL bResult = m_pIocp->GetStatus(&numberOfBytesTransffered, &completionKey, (LPOVERLAPPED*)&pOverlapped);

		IOCPPostOrder* pIOCPPostOrder = reinterpret_cast<IOCPPostOrder*>(completionKey);
		IOCPOverlapped* pIOCPOverlapped = static_cast<IOCPOverlapped*>(pOverlapped);		// dynamic_cast를 하고싶지만 OVERLAPPED는 가상 구조체가 아님

		if (pIOCPOverlapped) {
			// 각 오버랩 타입에 맞게 작업 처리
			pIOCPOverlapped->Process(bResult, numberOfBytesTransffered, pIOCPPostOrder);
			pIOCPOverlapped->Release();
			continue;
		}

		if (numberOfBytesTransffered == 0 && completionKey != NULL) {

			// 실제 로직처리는 IOCPPostOrder의 Process() 함수에서 진행
			// pIOCPPostOrder 메모리 해제는 Process 내부에서 처리
			switch (pIOCPPostOrder->Process(this)) {
			case IOCP_POST_ORDER_TERMINATE:
				goto THREAD_END;
			case IOCP_POST_ORDER_ERROR:
				DebugAssertMsg(false, "오류");
			default:
				DebugAssertMsg(false, "이상한 타입의 포스트 오더입니다.");
			}
		}

		break;
	}

THREAD_END:
	NetLog("IOCPWorker 쓰레드가 종료되었습니다. (%d)\n", Thread::GetThreadId());
	m_eState = State::JoinWait;
}

NS_JNET_END

