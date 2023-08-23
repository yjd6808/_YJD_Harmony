﻿/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JCore/Primitives/AutoPtr.h>

#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/IOCP/IOCPPostOrder.h>
#include <JNetwork/IOCP/IOCPWorker.h>

NS_JNET_BEGIN

int IOCPPostOrder::Process(IOCPWorker* worker) {
	switch (Order) {
	case IOCP_POST_ORDER_TERMINATE:
		Handle->Signal();
		return IOCP_POST_ORDER_TERMINATE;
	default: 
		return -1;
	}
}

NS_JNET_END




