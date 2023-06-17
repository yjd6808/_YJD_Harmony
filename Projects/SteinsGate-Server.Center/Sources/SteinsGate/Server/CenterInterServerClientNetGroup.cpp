/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:44:32 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterInterServerClientNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

CenterInterServerClientNetGroup::CenterInterServerClientNetGroup() {
}

void CenterInterServerClientNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void CenterInterServerClientNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void CenterInterServerClientNetGroup::InitializeInterServerTcp() {
	// 중앙 서버 자체가 인터서버 호스트이므로.. TCP 클라이언트는 사용하지 않는다.
}

void CenterInterServerClientNetGroup::InitializeInterServerUdp() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_InterServerClientUdpListener, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfoPackage_v->Center.BindInterServerUdp);
	AddHost(spInterServerClient);
	m_pInterServerClientUdp = spInterServerClient.Get<UdpClient*>();
	m_pInterServerClientUdp->RecvFromAsync();
}

void CenterInterServerClientNetGroup::OnLoop(JCore::PulserStatistics* pulseStat) {

}
