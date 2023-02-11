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
	, m_LoginListener("로그인 클라")
	, m_ChannelListener("채널 클라")
	, m_GameListener("게임 클라")
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

	TcpClientPtr spLoginClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_LoginListener);;
	TcpClientPtr spChannelClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_ChannelListener);
	TcpClientPtr spGameClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_GameListener);

	AddHost(spLoginClient);
	AddHost(spChannelClient);
	AddHost(spGameClient);

	m_spLoginClient = spLoginClient;
	m_spChannelClient = spChannelClient;
	m_spGameClient = spGameClient;
}

NS_JNET_END