#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/IOCP/IOCPWorker.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlapped.h>

using namespace JCore;

namespace JNetwork {
	IOCPWorker::IOCPWorker(IOCP* iocp) : 
		Worker(), 
		m_pIocp(iocp)
	{
		
	}

	IOCPWorker::~IOCPWorker() {
	}

	void IOCPWorker::Run(void* param) {
		// std::thread ��� �Լ��� �����ϴ� ��
		// @���� : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

		m_eState = State::Running;
		m_Thread = std::thread{ [this, param]() { WorkerThread(param); }};
	}

	void IOCPWorker::JoinWait(HANDLE waitHandle) {
		ULONG_PTR completionKey = (ULONG_PTR)new IOCPPostOrder{ IOCP_POST_ORDER_TERMINATE, waitHandle };
		m_pIocp->Post(0, completionKey, nullptr);	// ��� �����尡 �ƾ�� �𸣱� ������ ���⼭ join�� �����ϸ� �ȵ�
		
	}

	void IOCPWorker::Join(void) {
		if (m_Thread.joinable())
			m_Thread.join();
		m_eState = State::Joined;
	}

	void IOCPWorker::Pause(HANDLE waitHandle) {
		// ����!> �� ��Ŀ �����尡 Pause �Ȱ� �ƴ϶� IOCP���� �������� ������� ���ϳ��� Pause��
		ULONG_PTR completionKey = (ULONG_PTR)new IOCPPostOrder{ IOCP_POST_ORDER_PAUSE, waitHandle };
		ResetEvent(m_hPauseEvent);
		m_pIocp->Post(0, completionKey, nullptr);
	}

	void IOCPWorker::Resume() {
		SetEvent(m_hPauseEvent);
	}


	void IOCPWorker::WorkerThread(void* param) {
		Winsock::Message("IOCPWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());

		for (;;) {
			DWORD numberOfBytesTransffered;
			ULONG_PTR completionKey = NULL;
			OVERLAPPED* pOverlapped = nullptr;

			BOOL bResult = m_pIocp->GetStatus(&numberOfBytesTransffered, &completionKey, (LPOVERLAPPED*)&pOverlapped);

			IOCPPostOrder* pIOCPPostOrder = reinterpret_cast<IOCPPostOrder*>(completionKey);
			IOCPOverlapped* pIOCPOverlapped = static_cast<IOCPOverlapped*>(pOverlapped);

			if (numberOfBytesTransffered == 0 && completionKey != NULL) {

				// ���� ����ó���� IOCPPostOrder�� Process() �Լ����� ����
				// pIOCPPostOrder �޸� ������ Process ���ο��� ó��
				switch (pIOCPPostOrder->Process(this)) {
					case IOCP_POST_ORDER_TERMINATE: 
						goto THREAD_END;
					case IOCP_POST_ORDER_PAUSE: 
						continue;
				}
			}

			if (pIOCPOverlapped) {
				// ����
				// 1. ������ �����Ҵ� ������ ������ ���ο��� ����
				// 2. �� ������ Ÿ�Կ� �°� �۾� ó��
				pIOCPOverlapped->Process(bResult, numberOfBytesTransffered, pIOCPPostOrder);
				pIOCPOverlapped->Release();
				continue;
			} 

			break;
		}

	THREAD_END:
		Winsock::Message("IOCPWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());
		m_eState = State::JoinWait;
	}
}

