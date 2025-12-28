/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:44:32 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterInterServerClientNetGroup.h"

#include <SteinsGate/Common/ListenerInterServerClient.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

//////////////////////////////////////////////////////////////////////////////////////////
CenterInterServerClientNetGroup::CenterInterServerClientNetGroup()
{
	SetName("센터 인터서버");
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::InitializeIOCP()
{
	CreateIocp(2);
	RunIocp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::InitializeParser()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::InitializeInterServerTcp()
{
	// 중앙 서버 자체가 인터서버 호스트이므로.. TCP 클라이언트는 사용하지 않는다.
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::InitializeInterServerUdp()
{
	auto pInterServerClient = MakeShared<UdpClient>(pIocp_, pBufferPool_, nullptr, RecvBufferSize_v, SendBufferSize_v);
	pInterServerClient->Bind(Core::ServerProcessInfoPackage->center_.bindInterServerUdp_);
	AddHost(Const::Host::CenterInterServerUdpId, pInterServerClient);
	pInterServerClientUdp_ = pInterServerClient.Get<UdpClient*>();
	pInterServerClientUdp_->SetEventListener(dbg_new ListenerInterServerClient{ ServerProcessType::Center, pParser_ });
	pInterServerClientUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& _elapsed)
{
}
