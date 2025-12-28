/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "GameInterServerClientNetGroup.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/ListenerInterServerClient.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

//////////////////////////////////////////////////////////////////////////////////////////
GameInterServerClientNetGroup::GameInterServerClientNetGroup()
{
	SetName("게임 인터서버");
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameInterServerClientNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameInterServerClientNetGroup::InitializeIOCP()
{
	CreateIocp(2);
	RunIocp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameInterServerClientNetGroup::InitializeInterServerTcp()
{
	auto pInterServerClient = MakeShared<TcpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::GameServerProcessInfo->bindInterServerTcp_);
	AddHost(Const::Host::GameInterServerTcpId, pInterServerClient);

	pInterServerClientTcp_ = pInterServerClient.Get<TcpClient*>();
	pInterServerClientTcp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Game, pParser_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameInterServerClientNetGroup::InitializeInterServerUdp()
{
	auto pInterServerClient = MakeShared<UdpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::GameServerProcessInfo->bindInterServerUdp_);
	AddHost(Const::Host::GameInterServerUdpId, pInterServerClient);

	pInterServerClientUdp_ = pInterServerClient.Get<UdpClient*>();
	pInterServerClientUdp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Game, pParser_ });
	pInterServerClientUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& _elapsed)
{
}
