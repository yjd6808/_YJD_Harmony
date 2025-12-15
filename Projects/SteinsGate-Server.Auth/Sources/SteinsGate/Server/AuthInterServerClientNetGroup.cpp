/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:44:32 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthInterServerClientNetGroup.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/ListenerInterServerClient.h>
#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

#include <SteinsGate/Server/R_AUTHENTICATION.h>


USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

//////////////////////////////////////////////////////////////////////////////////////////
AuthInterServerClientNetGroup::AuthInterServerClientNetGroup()
{
	SetName("인증 인터서버");
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthInterServerClientNetGroup::~AuthInterServerClientNetGroup()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeParser()
{
	InterServerClientNetGroup::InitializeParser();

	// AUTHENTICATION
	parser_->AddCommand<SAU_AuthenticationCheck>(R_AUTHENTICATION::RECV_SAU_AuthenticationCheck);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeIOCP()
{
	CreateIocp(2);
	RunIocp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeInterServerTcp()
{
	auto pInterServerClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::ServerProcessInfoPackage->auth_.bindInterServerTcp_);
	AddHost(Const::Host::AuthInterServerTcpId, pInterServerClient);

	interServerClientTcp_ = pInterServerClient.Get<TcpClient*>();
	interServerClientTcp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Auth, parser_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeInterServerUdp()
{
	auto pInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::ServerProcessInfoPackage->auth_.bindInterServerUdp_);
	AddHost(Const::Host::AuthInterServerUdpId, pInterServerClient);

	interServerClientUdp_ = pInterServerClient.Get<UdpClient*>();
	interServerClientUdp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Auth, parser_ });
	interServerClientUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& _elapsed)
{
}
