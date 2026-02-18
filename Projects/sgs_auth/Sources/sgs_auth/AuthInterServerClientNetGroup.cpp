/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:44:32 PM
 * =====================
 *
 */


#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthInterServerClientNetGroup.h"

#include <sgs/CmdHost.h>
#include <sgs/_Net/NetClientListener_InterServ.h>
#include <sgs/CmdRelay_AUTHENTICATION.h>

#include <sgs_auth/R_AUTHENTICATION.h>


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
	NetGroup_InterServ::InitializeParser();

	// AUTHENTICATION
	pParser_->AddCommand<SAU_AuthenticationCheck>(R_AUTHENTICATION::RECV_SAU_AuthenticationCheck);
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
	auto pInterServerClient = MakeShared<TcpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(sg::ServerProcessInfoPackage->auth_.bindInterServerTcp_);
	AddHost(Const::Host::AuthInterServerTcpId, pInterServerClient);

	pInterServerClientTcp_ = pInterServerClient.Get<TcpClient*>();
	pInterServerClientTcp_->SetEventListener(dbg_new NetClientListener_InterServ{ ServerProcessType::Auth, pParser_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::InitializeInterServerUdp()
{
	auto pInterServerClient = MakeShared<UdpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(sg::ServerProcessInfoPackage->auth_.bindInterServerUdp_);
	AddHost(Const::Host::AuthInterServerUdpId, pInterServerClient);

	pInterServerClientUdp_ = pInterServerClient.Get<UdpClient*>();
	pInterServerClientUdp_->SetEventListener(dbg_new NetClientListener_InterServ{ ServerProcessType::Auth, pParser_ });
	pInterServerClientUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthInterServerClientNetGroup::OnUpdate(const jc::TimeSpan& _elapsed)
{
}
