/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:05:43 AM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "InterServerSendHelper.h"

USING_NS_JNET;

InterServerSendHelperBase::Information::Information()
	: Sender(nullptr)
	, Strategy(SendStrategy::SendAlloc)
	, ToId(InvalidValue_v)
{}

void InterServerSendHelperBase::FlushSendBuffer() {
	if (SendInformation.Sender == nullptr) {
		_LogError_("샌더 미할당");
		return;
	}

	SendInformation.Sender->FlushSendBuffer();
}

void InterServerSendHelperBase::SetInformation(Session* sender, SendStrategy strategy, int toServerId) {
	if (!IsValidInformation(sender, strategy, toServerId)) {
		return;
	}

	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;

	if (toServerId == InvalidValue_v) 
		return;

	SendInformation.ToId = toServerId;
	SendInformation.Destination = Core::ServerProcessInfoPackage->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
}

void InterServerSendHelperBase::SetInformation(Session* sender, SendStrategy strategy, SingleServerType_t toServerType) {
	DebugAssert(toServerType >= SingleServerType::Begin && toServerType <= SingleServerType::End);

	if (!SendHelperBase::IsValidInformation(sender, strategy)) {
		return;
	}

	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;
	SendInformation.ToId = SingleServerId[toServerType];
	SendInformation.Destination = Core::ServerProcessInfoPackage->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
}

void InterServerSendHelperBase::SendEnd(IPacket* packet) {
	switch (SendInformation.Strategy) {
	case SendStrategy::SendAsync:
		SendInformation.Sender->SendAsync(packet);
		break;
	case SendStrategy::SendToAsync:
		SendInformation.Sender->SendToAsync(packet, SendInformation.Destination);
		break;
	case SendStrategy::SendAlloc:
		// 할거 없음
		break;
	default:
		_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
	}
}

bool InterServerSendHelperBase::IsValidInformation(Session* sender, SendStrategy strategy, int toServerId) {

	if (!SendHelperBase::IsValidInformation(sender, strategy)) {
		return false;
	}

	if (sender->Protocol() == TransportProtocol::UDP) {
		if (toServerId == InvalidValue_v) {
			_LogWarn_("UDP인데 송신지 ID를 기입하지 않습니다.");
			return false;
		}
	}

	return true;
}

void InterServerSendHelperBase::InitSingleServerIds() {
	SingleServerId[SingleServerType::Center] = Core::ServerProcessInfoPackage->Center.ServerId;
	SingleServerId[SingleServerType::Auth] = Core::ServerProcessInfoPackage->Auth.ServerId;
	SingleServerId[SingleServerType::Lobby] = Core::ServerProcessInfoPackage->Lobby.ServerId;
}

void InterServerSendHelperBase::InitSingleServerDestinations() {
	SingleServerInterServerEP[SingleServerType::Center] = Core::ServerProcessInfoPackage->Center.RemoteInterServerEP;
	SingleServerInterServerEP[SingleServerType::Auth] = Core::ServerProcessInfoPackage->Auth.RemoteInterServerEP;
	SingleServerInterServerEP[SingleServerType::Lobby] = Core::ServerProcessInfoPackage->Lobby.RemoteInterServerEP;
}

int InterServerSendHelperBase::GetSenderId() {
	const int iSenderId = Core::ServerProcessInfo->ServerId;
	DebugAssert(iSenderId >= 0 && iSenderId <= Const::Server::MaxProcessId);
	return int(iSenderId);
}


