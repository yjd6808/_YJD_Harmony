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

//////////////////////////////////////////////////////////////////////////////////////////
TcpClientNetGroup::TcpClientNetGroup()
	: NetGroup("테스트 클라이언트 그룹")
{
}

//////////////////////////////////////////////////////////////////////////////////////////
TcpClientNetGroup::~TcpClientNetGroup()
{
	TcpClientNetGroup::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void TcpClientNetGroup::Initialize()
{
	CreateIocp(8);
	CreateBufferPool({
		{ 8192, 10 }
	});

	RunIocp();

	TcpClientPtr pLoginClient = MakeShared<TcpClient>(pIocp_, pBufferPool_);
	TcpClientPtr pChannelClient = MakeShared<TcpClient>(pIocp_, pBufferPool_);
	TcpClientPtr pGameClient = MakeShared<TcpClient>(pIocp_, pBufferPool_);

	AddHost(1, pLoginClient);
	AddHost(2, pChannelClient);
	AddHost(3, pGameClient);

	loginClient_ = pLoginClient;
	loginClient_->SetEventListener(dbg_new ClientListener{ "로그인 클라" });
	channelClient_ = pChannelClient;
	channelClient_->SetEventListener(dbg_new ClientListener{ "채널 클라" });
	gameClient_ = pGameClient;
	gameClient_->SetEventListener(dbg_new ClientListener{ "게임 클라" });
}

NS_JNET_END
