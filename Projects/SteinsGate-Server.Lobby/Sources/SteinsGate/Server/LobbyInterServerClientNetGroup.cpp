/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyInterServerClientNetGroup.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/ListenerInterServerClient.h>
#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

#include <SteinsGate/Server/R_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

//////////////////////////////////////////////////////////////////////////////////////////
LobbyInterServerClientNetGroup::LobbyInterServerClientNetGroup()
{
	SetName("로비 인터서버");
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::InitializeIOCP()
{
	CreateIocp(2);
	RunIocp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::InitializeParser()
{
	InterServerClientNetGroup::InitializeParser();

	// AUTHENTICATION
	pParser_->AddCommand<AUS_AuthenticationCheckAck>(R_AUTHENTICATION::RECV_AUS_AuthenticationCheckAck);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::InitializeInterServerTcp()
{
	auto pInterServerClient = MakeShared<TcpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::ServerProcessInfoPackage->lobby_.bindInterServerTcp_);
	AddHost(Const::Host::LobbyInterServerTcpId, pInterServerClient);

	pInterServerClientTcp_ = pInterServerClient.Get<TcpClient*>();
	pInterServerClientTcp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Lobby, pParser_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::InitializeInterServerUdp()
{
	auto pInterServerClient = MakeShared<UdpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::ServerProcessInfoPackage->lobby_.bindInterServerUdp_);
	AddHost(Const::Host::LobbyInterServerUdpId, pInterServerClient);

	pInterServerClientUdp_ = pInterServerClient.Get<UdpClient*>();
	pInterServerClientUdp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Lobby, pParser_ });
	pInterServerClientUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& _elapsed)
{
}
