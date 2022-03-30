// ReSharper disable CppClangTidyCppcoreguidelinesProTypeStaticCastDowncast
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/IOCP/IOCPWorker.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

using namespace JCore;

namespace JNetwork {
	IOCPWorker::IOCPWorker(IOCP* iocp) : 
		Worker(), 
		m_pIocp(iocp) {
	}

	IOCPWorker::~IOCPWorker() = default;

	void IOCPWorker::Run(void* param) {
		// std::thread 멤버 함수로 실행하는 법
		// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

		m_eState = State::Running;
		m_Thread = std::thread{ [this, param]() { WorkerThread(param); }};
	}

	void IOCPWorker::JoinWait(HANDLE waitHandle) {
		const ULONG_PTR completionKey = (ULONG_PTR)new IOCPPostOrder{ IOCP_POST_ORDER_TERMINATE, waitHandle };
		m_pIocp->Post(0, completionKey, nullptr);	// 어느 쓰레드가 꺠어날지 모르기 때문에 여기서 join을 수행하면 안됨
		
	}

	void IOCPWorker::Join() {
		if (m_Thread.joinable())
			m_Thread.join();
		m_eState = State::Joined;
	}

	void IOCPWorker::Pause(HANDLE waitHandle) {
		// 주의!> 이 워커 쓰레드가 Pause 된게 아니라 IOCP에서 관리중인 쓰레드들 중하나가 Pause됨
		const ULONG_PTR completionKey = (ULONG_PTR)new IOCPPostOrder{ IOCP_POST_ORDER_PAUSE, waitHandle };
		ResetEvent(m_hPauseEvent);
		m_pIocp->Post(0, completionKey, nullptr);
	}

	void IOCPWorker::Resume() {
		SetEvent(m_hPauseEvent);
	}


	void IOCPWorker::WorkerThread(void* param) {
		Winsock::Message("IOCPWorker 쓰레드가 실행되었습니다. (%d)", std::this_thread::get_id());

		for (;;) {
			DWORD numberOfBytesTransffered;
			ULONG_PTR completionKey = NULL;
			OVERLAPPED* pOverlapped = nullptr;

			const BOOL bResult = m_pIocp->GetStatus(&numberOfBytesTransffered, &completionKey, (LPOVERLAPPED*)&pOverlapped);

			IOCPPostOrder* pIOCPPostOrder = reinterpret_cast<IOCPPostOrder*>(completionKey);
			IOCPOverlapped* pIOCPOverlapped = static_cast<IOCPOverlapped*>(pOverlapped);		// dynamic_cast를 하고싶지만 OVERLAPPED는 가상 구조체가 아님

			if (numberOfBytesTransffered == 0 && completionKey != NULL) {

				// 실제 로직처리는 IOCPPostOrder의 Process() 함수에서 진행
				// pIOCPPostOrder 메모리 해제는 Process 내부에서 처리
				switch (pIOCPPostOrder->Process(this)) {
					case IOCP_POST_ORDER_TERMINATE: 
						goto THREAD_END;
					case IOCP_POST_ORDER_PAUSE:
						continue;
					case IOCP_POST_ORDER_ERROR:
						DebugAssert(false, "... WTF?");
				}
			}

			if (pIOCPOverlapped) {
				// 역할
				// 1. 오버랩 동정할당 해제는 오버랩 내부에서 수행
				// 2. 각 오버랩 타입에 맞게 작업 처리
				pIOCPOverlapped->Process(bResult, numberOfBytesTransffered, pIOCPPostOrder);
				pIOCPOverlapped->Release();
				continue;
			} 

			break;
		}

	THREAD_END:
		Winsock::Message("IOCPWorker 쓰레드가 종료되었습니다. (%d)", std::this_thread::get_id());
		m_eState = State::JoinWait;
	}
}

