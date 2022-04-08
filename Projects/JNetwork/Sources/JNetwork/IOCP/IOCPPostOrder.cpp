/*
 * �ۼ��� : ������
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JCore/AutoObject.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCP/IOCPWorker.h>

namespace JNetwork {

int IOCPPostOrder::Process(IOCPWorker* worker) {
	JCore::AutoPointer<IOCPPostOrder> autoPtrRelease(this, [](IOCPPostOrder* po) {po->Release(); });

	switch (Order) {
	case IOCP_POST_ORDER_TERMINATE:
		SetEvent(Handle);
		return IOCP_POST_ORDER_TERMINATE;

	case IOCP_POST_ORDER_PAUSE:
		worker->m_eState = Worker::State::Paused;
		Winsock::Message("IOCPWorker �����尡 �Ͻ� �����Ǿ����ϴ�. (%d)", std::this_thread::get_id());
		SetEvent(Handle);										// WorkerManager�� WaitForMultipleObjects�� ��ȣ�� ������ / �ش� �����尡 �����Ǿ����� �˷��ش�.
		WaitForSingleObject(worker->m_hPauseEvent, INFINITE);	// ��Ŀ�������� �̺�Ʈ �ڵ�� �� �����带 ��������
		Winsock::Message("IOCPWorker �����带 �ٽ� ��� �����մϴ�. (%d)", std::this_thread::get_id());
		SetEvent(Handle);										// WorkerManager�� WaitForMultipleObjects�� ��ȣ�� ������ / �ش� �����尡 ��� ������ ���������� �˷��ش�.
		worker->m_eState = Worker::State::Running;
		return IOCP_POST_ORDER_PAUSE;
	default: 
		return -1;
	}
}

}



