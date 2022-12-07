/*
 * 작성자 : 윤정도
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
		// "IOCPWorker 쓰레드가 일시 정지되었습니다. (%d)", std::this_thread::get_id()
		SetEvent(Handle);										// WorkerManager의 WaitForMultipleObjects에 신호를 보내줌 / 해당 쓰레드가 정지되었음을 알려준다.
		WaitForSingleObject(worker->m_hPauseEvent, INFINITE);	// 워커쓰레드의 이벤트 핸들로 이 쓰레드를 정지해줌
		// "IOCPWorker 쓰레드를 다시 계속 실행합니다. (%d)", std::this_thread::get_id()
		SetEvent(Handle);										// WorkerManager의 WaitForMultipleObjects에 신호를 보내줌 / 해당 쓰레드가 계속 진행을 시작했음을 알려준다.
		worker->m_eState = Worker::State::Running;
		return IOCP_POST_ORDER_PAUSE;
	default: 
		return -1;
	}
}

}



