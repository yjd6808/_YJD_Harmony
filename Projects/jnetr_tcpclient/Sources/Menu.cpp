/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 10:32:50 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Core.h>
#include <jnet/Winsock.h>
#include <jnetr_common/TcpClientNetGroup.h>
#include <jnetr_common/Command.h>
#include <jnetr_common/Config.h>

#include "Menu.h"

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

extern TcpClientNetGroup* pClientGroup;

static void SendMsg(TcpClient* _pClient);

void ConnectLogin()
{
	// pClientGroup->GetLogin()->ConnectAsync(ServerLoginTcpAddr);
	pClientGroup->GetLogin()->Connect(IPv4EndPoint{ SERVER_LOGIN_TCP_ADDR }, 1000);
}

void ConnectChannel()
{
	// pClientGroup->GetChannel()->ConnectAsync(ServerChannelTcpAddr);
	pClientGroup->GetLogin()->Connect(IPv4EndPoint{ SERVER_CHANNEL_TCP_ADDR }, 1000);
}

void ConnectGame()
{
	// pClientGroup->GetGame()->ConnectAsync(ServerGameTcpddr);
	pClientGroup->GetLogin()->Connect(IPv4EndPoint{ SERVER_GAME_TCP_ADDR }, 1000);
}

void SendLogin()
{
	SendMsg(pClientGroup->GetLogin());
}

void SendChannel()
{
	SendMsg(pClientGroup->GetChannel());
}

void SendGame()
{
	SendMsg(pClientGroup->GetGame());
}

void DisconnectLogin()
{
	pClientGroup->GetLogin()->Disconnect();
}

void DisconnectChannel()
{
	pClientGroup->GetChannel()->Disconnect();
}

void DisconnectGame()
{
	pClientGroup->GetGame()->Disconnect();
}


static void SendMsg(TcpClient* _pClient)
{
	StaticString<512> inputString;
	cout << "보낼 메시지 입력: ";

	if (!(cin >> inputString.Source).good())
	{
		cout << "제대로 입력해주세요.\n";
		return;
	}

	int length = inputString.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 = _pClient->EnqueueCmd<StaticMessage>();
	DynamicMessage& msg2 = _pClient->EnqueueCmd<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.msg_.Source, inputString.Source);
	StringUtil::CopyUnsafe(msg2.Msg(), inputString.Source);

	// 다이나믹 패킷을 활용한 전송
	auto pPacket1 = dbg_new DynamicCmdPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto pMsg3 = pPacket1->Get<0>();
	auto pMsg4 = pPacket1->Get<1>();
	auto pMsg5 = pPacket1->Get<2>();
	StringUtil::CopyUnsafe(pMsg3->Msg(), inputString.Source);
	StringUtil::CopyUnsafe(pMsg4->Msg(), inputString.Source);
	StringUtil::CopyUnsafe(pMsg5->Msg(), inputString.Source);
	_pClient->SendAsync(pPacket1);

	// 커맨드 버퍼를 활용한 전송 테스트
	PacketBufferPtr pBuffer = PacketBuffer::Create(_pClient->GetBufferAllocator());
	DynamicMessage& msg6 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg7 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg8 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	StringUtil::CopyUnsafe(msg6.Msg(), inputString.Source);
	StringUtil::CopyUnsafe(msg7.Msg(), inputString.Source);
	StringUtil::CopyUnsafe(msg8.Msg(), inputString.Source);
	_pClient->SendAsync(pBuffer);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(length);
	StringUtil::CopyUnsafe(pMsg9->cmd_.Msg(), inputString.Source);
	_pClient->SendAsync(pMsg9);
	
	// 스태틱 패킷 전송
	auto pPacket = dbg_new StaticCmdPacket<StaticMessage>();
	StaticMessage* pArg1 = pPacket->Get<0>();
	pArg1->msg_.SetString(inputString);
	auto wsaBuf = pPacket->GetWSABuf();
	_pClient->SendAsync(pPacket);

	_pClient->FlushSendBuffer();


	jc::CMessage legMsg = _pClient->EnqueueMsg();
	legMsg.WriteU32(1234);
	legMsg.WriteString("abcdefg");

	jc::CMessage legMsg2 = _pClient->EnqueueMsg();
	legMsg2.WriteU32(5678);
	legMsg2.WriteString("hijklmn");

	_u32 aaa = legMsg.ReadU32();
	String bbb = legMsg.ReadString();

	aaa = legMsg2.ReadU32();
	bbb = legMsg2.ReadString();


	int a = 30;


}
