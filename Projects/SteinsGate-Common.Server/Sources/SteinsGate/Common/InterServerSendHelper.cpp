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

void InterServerSendHelperBase::SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, int toServerId) {
	if (!IsValidInformation(sender, strategy, toServerId)) {
		return;
	}

	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;

	if (toServerId == InvalidValue_v) 
		return;

	SendInformation.ToId = toServerId;
	SendInformation.Destination = CoreServerProcessInfoPackage_v->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
}

void InterServerSendHelperBase::SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, SingleServerType_t toServerType) {
	DebugAssert(toServerType >= SingleServerType::Begin && toServerType <= SingleServerType::End);

	if (!IsValidInformation(sender, strategy)) {
		return;
	}

	SendInformation.Sender = sender;
	SendInformation.Strategy = strategy;
	SendInformation.ToId = SingleServerId[toServerType];
	SendInformation.Destination = CoreServerProcessInfoPackage_v->InfoMap[SendInformation.ToId]->RemoteInterServerEP;
}

void InterServerSendHelperBase::SetReceiverId(int serverId) {
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

bool InterServerSendHelperBase::IsValidInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, int toServerId) {

	const JNetwork::TransportProtocol eProtocol = sender->Protocol();

	if (eProtocol == JNetwork::TransportProtocol::TCP) {

		if (strategy == JNetwork::eSendToAsync || strategy == JNetwork::eSendToAsyncEcho) {
			_LogWarn_("TCP인데 UDP 송신전략을 사용할려고합니다.");
			return false;
		}

		// 릴레이패킷일 수 있으므로 TCP도 toServerId를 지정할 수도 있음
	}

	if (eProtocol == JNetwork::TransportProtocol::UDP) {

		if (strategy == JNetwork::eSendAlloc || strategy == JNetwork::eSendAsync) {
			_LogWarn_("UDP인데 TCP 송신전략을 사용할려고합니다.");
			return false;
		}

		if (toServerId == InvalidValue_v) {
			_LogWarn_("UDP인데 송신지 ID를 기입하지 않습니다.");
			return false;
		}
	}

	return true;
}

bool InterServerSendHelperBase::IsValidInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy) {

	const JNetwork::TransportProtocol eProtocol = sender->Protocol();

	if (eProtocol == JNetwork::TransportProtocol::TCP) {

		if (strategy == JNetwork::eSendToAsync || strategy == JNetwork::eSendToAsyncEcho) {
			_LogWarn_("TCP인데 UDP 송신전략을 사용할려고합니다.");
			return false;
		}
	}

	if (eProtocol == JNetwork::TransportProtocol::UDP) {

		if (strategy == JNetwork::eSendAlloc || strategy == JNetwork::eSendAsync) {
			_LogWarn_("UDP인데 TCP 송신전략을 사용할려고합니다.");
			return false;
		}
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

void InterServerSendHelperBase::InitDefaultToId(int id) {
	DefaultToId = id;
}

int InterServerSendHelperBase::GetSenderId() {
	const int iSenderId = CoreServerProcessInfo_v->ServerId;
	DebugAssert(iSenderId >= 0 && iSenderId <= Const::Server::MaxId);
	return int(iSenderId);
}


