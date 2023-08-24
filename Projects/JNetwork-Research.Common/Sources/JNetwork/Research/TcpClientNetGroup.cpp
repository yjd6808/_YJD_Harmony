/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:13:58 PM
 * =====================
 *
 */

#include "Research.h"
#include <JNetwork/Research/TcpClientNetGroup.h>
#include <JNetwork/Research/Command.h>

USING_NS_JC;

NS_JNET_BEGIN

TcpClientNetGroup::TcpClientNetGroup()
	: NetGroup("테스트 클라이언트 그룹")
{}

TcpClientNetGroup::~TcpClientNetGroup() {
	TcpClientNetGroup::Finalize();
}

void TcpClientNetGroup::Initialize() {
	CreateIocp(8);
	CreateBufferPool({
		{ 8192, 10 }
	});

	RunIocp();

	TcpClientPtr spLoginClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool);
	TcpClientPtr spChannelClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool);
	TcpClientPtr spGameClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool);

	AddHost(1, spLoginClient);
	AddHost(2, spChannelClient);
	AddHost(3, spGameClient);

	m_spLoginClient = spLoginClient;
	m_spLoginClient->SetEventListener(dbg_new ClientListener{ "로그인 클라" });
	m_spChannelClient = spChannelClient;
	m_spChannelClient->SetEventListener(dbg_new ClientListener{ "채널 클라" });
	m_spGameClient = spGameClient;
	m_spGameClient->SetEventListener(dbg_new ClientListener{ "게임 클라" });
}

NS_JNET_END