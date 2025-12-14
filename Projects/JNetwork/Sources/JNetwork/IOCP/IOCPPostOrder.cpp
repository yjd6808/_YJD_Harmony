/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JCore/Primitives/AutoPtr.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCP/IOCPWorker.h>

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
int IOCPPostOrder::Process(IOCPWorker* _pWorker)
{
	(void)_pWorker;

	switch (order_)
	{
	case IOCP_POST_ORDER_TERMINATE:
		waitHandle_->Signal();
		return IOCP_POST_ORDER_TERMINATE;
	default:
		return IOCP_POST_ORDER_ERROR;
	}
}

NS_JNET_END
