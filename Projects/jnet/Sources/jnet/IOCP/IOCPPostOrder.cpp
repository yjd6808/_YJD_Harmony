/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Winsock.h>

#include <jc/Primitives/AutoPtr.h>

#include <jnet/IOCP/IOCP.h>
#include <jnet/IOCP/IOCPPostOrder.h>
#include <jnet/IOCP/IOCPWorker.h>

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
