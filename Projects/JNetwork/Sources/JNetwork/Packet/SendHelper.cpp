/*
 * 작성자: 윤정도
 * 생성일: 4/24/2023 4:55:19 PM
 * =====================
 * 다양한 송신 방식을 하나로 통합하기 위함. */

#include <JNetwork/Network.h>
#include <JNetwork/Packet/SendHelper.h>

USING_NS_JC;

NS_JNET_BEGIN

void SendHelper::FlushSendBuffer() {
	if (SendInformation.Sender == nullptr) {
		_LogError_("샌더 미할당");
		return;
	}

	SendInformation.Sender->FlushSendBuffer();
}

void SendHelper::SetInformation(Session* sender) {
	SendInformation.Sender = sender;
}

void SendHelper::SetInformation(Session* sender, SendStrategy strategy) {
	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;
}

void SendHelper::SetInformation(Session* sender, SendStrategy strategy, const IPv4EndPoint& destination) {
	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;
	SendInformation.Destination = destination;
}

void SendHelper::SendEnd(ISendPacket* packet) {
	if (SendInformation.Strategy == eSendAsync) {
		SendInformation.Sender->SendAsync(packet);
	} else if (SendInformation.Strategy == eSendToAsync) {
		SendInformation.Sender->SendToAsync(packet, SendInformation.Destination);
	} else if (SendInformation.Strategy == eSendToAsyncEcho) {
		SendInformation.Sender->SendToAsyncEcho(packet);
	} else if (SendInformation.Strategy == eSendAlloc) {
		// 할거없음
	} else {
		_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
	}
}

NS_JNET_END
