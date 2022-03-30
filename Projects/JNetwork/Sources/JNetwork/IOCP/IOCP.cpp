#include <JNetwork/Network.h>
#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/WorkerManager.h>
#include <JNetwork/IOCP/IOCPWorker.h>

namespace JNetwork {

	IOCP::IOCP() :
		m_eState(State::Uninitialized),
		m_hIOCP(INVALID_HANDLE_VALUE),
		m_iThreadCount(0),
		m_pWorkerManager(nullptr)
	{
	}

	IOCP::~IOCP()  {
		if (m_eState == State::Running) {
			DebugAssert(false, "먼저 조인을 해주세요");
		}

		if (m_eState != State::Destroyed && !Destroy()) {
			DebugAssert(false, "IOCP 삭제 실패");
		}
	}


	bool IOCP::Create(int threadCount) {
		if ((m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, (DWORD)threadCount)) == INVALID_HANDLE_VALUE) {
			return false;
		}

		m_iThreadCount = threadCount;
		m_pWorkerManager = WorkerManager::Create<IOCPWorker>(threadCount, this);
		m_eState = State::Initialized;
		return true;
	}

	bool IOCP::Destroy() {
		if (m_eState == State::Paused || 
			m_eState == State::Running || 
			m_eState == State::Destroyed) {
			return false;
		}

		if (m_hIOCP != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hIOCP);
			m_hIOCP = INVALID_HANDLE_VALUE;
		}

		DeleteSafe(m_pWorkerManager);
		m_eState = State::Destroyed;
		return true;
	}

	void IOCP::Run() {
		if (m_eState != State::Initialized) {
			DebugAssert(false, "Initialized 상태의 IOCP만 Run할 수 있습니다.");
			return;
		}

		m_pWorkerManager->Run();
		m_eState = State::Running;
	}

	void IOCP::Pause() {
		if (m_eState != State::Running) {
			DebugAssert(false, "Running 상태의 IOCP만 Pause할 수 있습니다.");
			return;
		}

		m_pWorkerManager->Pause();
		m_eState = State::Paused;
	}

	void IOCP::Resume() {
		if (m_eState != State::Paused) {
			DebugAssert(false, "Paused 상태의 IOCP만 Resume할 수 있습니다.");
			return;
		}

		m_pWorkerManager->Resume();
		m_eState = State::Running;
	}

	void IOCP::Join() {
		if (m_eState != State::Running && m_eState != State::Paused) {
			DebugAssert(false, "Paused 또는 Running 상태의 IOCP만 Join 할 수 있습니다.");
			return;
		}

		// 일시정지 상태인 경우 모두 진행시켜주자.
		if (m_eState == State::Paused) {
			m_pWorkerManager->Resume();
		}

		m_pWorkerManager->Join();
		m_eState = State::Joined;
	}

	bool IOCP::Connect(HANDLE handle, ULONG_PTR completionKey) const {
		return CreateIoCompletionPort(handle, m_hIOCP, completionKey, m_iThreadCount) != 0;
	}

	BOOL IOCP::GetStatus(LPDWORD numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped) const {
		return GetQueuedCompletionStatus(m_hIOCP, numberOfBytesTransffered, completionKey, ppOverlapped, INFINITE);
	}

	BOOL IOCP::Post(DWORD dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped) const {
		return PostQueuedCompletionStatus(m_hIOCP, dwNumberOfBytesTransferred, dwCompletionKey, pOverlapped);
	}
}

