/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:05:43 AM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "InterServerSendHelper.h"


InterServerSendHelperBase::Information::Information()
	: Sender(nullptr)
	, Strategy(JNetwork::eSendAlloc)
	, ToId(InvalidValue_v)
{}

void InterServerSendHelperBase::FlushSendBuffer() {
	if (SendInformation.Sender == nullptr) {
		_LogError_("샌더 미할당");
		return;
	}

	SendInformation.Sender->FlushSendBuffer();
}

void InterServerSendHelperBase::SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, Int8 toServerId) {
	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;

	if (toServerId == InvalidValue_v)
		return;

	SendInformation.ToId = toServerId;

	if (IsUDPStrategy(strategy)) {
		DebugAssertMsg(sender->IsUDP(), "TCP 세션으로 UDP 송신 전략을 사용하였습니다.");
		SendInformation.Destination = CoreServerProcessInfoPackage_v->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
	} else {
		DebugAssertMsg(sender->IsTCP(), "UDP 세션으로 TCP 송신 전략을 사용하였습니다.");
	}
}

void InterServerSendHelperBase::SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, SingleServerType_t singleServerType) {
	DebugAssert(singleServerType >= SingleServerType::Begin && singleServerType <= SingleServerType::End);

	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;
	SendInformation.ToId = SingleServerId[singleServerType];

	if (IsUDPStrategy(strategy)) {
		DebugAssertMsg(sender->IsUDP(), "TCP 세션으로 UDP 송신 전략을 사용하였습니다.");
		SendInformation.Destination = CoreServerProcessInfoPackage_v->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
	} else {
		DebugAssertMsg(sender->IsTCP(), "UDP 세션으로 TCP 송신 전략을 사용하였습니다.");
	}
}

void InterServerSendHelperBase::SetReceiverId(Int8 serverId) {
	SendInformation.ToId = serverId;
}

void InterServerSendHelperBase::SetStrategy(JNetwork::SendStrategy strategy) {
	SendInformation.Strategy = strategy;
}

void InterServerSendHelperBase::SendEnd(JNetwork::ISendPacket* packet) {
	switch (SendInformation.Strategy) {
	case JNetwork::eSendAsync:
		SendInformation.Sender->SendAsync(packet);
		break;
	case JNetwork::eSendToAsync:
		SendInformation.Sender->SendToAsync(packet, SendInformation.Destination);
		break;
	case JNetwork::eSendToAsyncEcho:
		SendInformation.Sender->SendToAsyncEcho(packet);
		break;
	case JNetwork::eSendAlloc:
		// 할거 없음
		break;
	default:
		_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
	}
}

bool InterServerSendHelperBase::IsUDPStrategy(JNetwork::SendStrategy strategy) {
	if (strategy == JNetwork::eSendToAsync)
		return true;
	if (strategy == JNetwork::eSendToAsyncEcho)
		return true;
	return false;
}


void InterServerSendHelperBase::InitSingleServerIds() {
	SingleServerId[SingleServerType::Center] = CoreServerProcessInfoPackage_v->Center.ServerId;
	SingleServerId[SingleServerType::Auth] = CoreServerProcessInfoPackage_v->Auth.ServerId;
	SingleServerId[SingleServerType::Lobby] = CoreServerProcessInfoPackage_v->Lobby.ServerId;
}

void InterServerSendHelperBase::InitSingleServerDestinations() {
	SingleServerInterServerEP[SingleServerType::Center] = CoreServerProcessInfoPackage_v->Center.RemoteInterServerEP;
	SingleServerInterServerEP[SingleServerType::Auth] = CoreServerProcessInfoPackage_v->Auth.RemoteInterServerEP;
	SingleServerInterServerEP[SingleServerType::Lobby] = CoreServerProcessInfoPackage_v->Lobby.RemoteInterServerEP;
}

void InterServerSendHelperBase::InitDefaultToId(Int8 id) {
	DefaultToId = id;
}

Int8 InterServerSendHelperBase::GetSenderId() {
	const int iSenderId = CoreServerProcessInfo_v->ServerId;
	DebugAssert(iSenderId >= 0 && iSenderId <= MaxServerId_v);
	return Int8(iSenderId);
}


