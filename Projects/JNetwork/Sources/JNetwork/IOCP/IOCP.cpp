/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/WorkerGroup.h>
#include <JNetwork/IOCP/IOCPWorker.h>

NS_JNET_BEGIN

IOCP::IOCP(int threadCount)
	: m_eState(State::eInitialized)
	, m_hIOCP(INVALID_HANDLE_VALUE)
	, m_uiThreadCount(threadCount)
	, m_pWorkerManager(nullptr)
{
	if ((m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, (Int32UL)threadCount)) == INVALID_HANDLE_VALUE) {
		DebugAssertMsg(false, "IOCP를 생성하는데 실패했습니다.");
	}

	m_uiThreadCount = threadCount;
	m_pWorkerManager = WorkerGroup::Create<IOCPWorker>(threadCount, this);
	m_eState = State::eInitialized;
}

IOCP::~IOCP()  {
	if (m_eState == State::eRunning) {
		DebugAssertMsg(false, "먼저 조인을 해주세요");
	}

	if (m_eState != State::eDestroyed) {
		Destroy();
	}
}



void IOCP::Destroy() {
	if (m_eState == State::eDestroyed) {
		return;
	}

	if (m_hIOCP != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hIOCP);
		m_hIOCP = INVALID_HANDLE_VALUE;
	}

	DeleteSafe(m_pWorkerManager);
	m_eState = State::eDestroyed;
}

void IOCP::Run() {
	if (m_eState != State::eInitialized) {
		DebugAssertMsg(false, "Initialized 상태의 IOCP만 Run할 수 있습니다.");
		return;
	}

	m_pWorkerManager->Run();
	m_eState = State::eRunning;
}

void IOCP::Join() {
	if (m_eState != State::eRunning) {
		DebugAssertMsg(false, "Running 상태의 IOCP만 Join 할 수 있습니다.");
		return;
	}

	// 펜딩카운트가 0이 될때까지 기다린다.
	WaitForZeroPending();

	m_pWorkerManager->Join();
	m_eState = State::eJoined;
}

void IOCP::WaitForZeroPending() {
	while (true) {

		int iPending = m_iPendingOverlappedCount;

		if (iPending == 0)
			break;

		DebugAssertMsg(iPending >= 0, "멍미 펜딩 카운트가 움수 인뎁쇼 (%d)", iPending);
		JCore::Thread::Sleep(10);
	}
}

bool IOCP::Connect(WinHandle handle, ULONG_PTR completionKey) const {
	return CreateIoCompletionPort(handle, m_hIOCP, completionKey, m_uiThreadCount) != 0;
}

BOOL IOCP::GetStatus(Int32UL* numberOfBytesTransffered, PULONG_PTR completionKey, LPOVERLAPPED* ppOverlapped) const {
	return GetQueuedCompletionStatus(m_hIOCP, numberOfBytesTransffered, completionKey, ppOverlapped, INFINITE);
}

BOOL IOCP::Post(Int32UL dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED pOverlapped) const {
	return PostQueuedCompletionStatus(m_hIOCP, dwNumberOfBytesTransferred, dwCompletionKey, pOverlapped);
}

NS_JNET_END

