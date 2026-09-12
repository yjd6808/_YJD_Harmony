/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 10:32:50 PM
 * =====================
 *
 */


#pragma once

#include "jnetr_common/PCH/Core.h"
#include "jnetr_common/TcpClientNetGroup.h"
#include "jnetr_common/Command.h"
#include "jnetr_common/Message.h"
#include "jnetr_common/Config.h"

#include "Menu.h"

#include "jc/Primitives/StringConvert.h"

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
#ifdef _UNICODE
	wcout << _T("보낼 메시지 입력: ");

	if (!(wcin >> inputString.Source).good())
	{
		wcout << _T("제대로 입력해주세요.\n");
		return;
	}
#else
	cout << "보낼 메시지 입력: ";

	if (!(cin >> inputString.Source).good())
	{
		cout << "제대로 입력해주세요.\n";
		return;
	}
#endif

	// dynamic narrow 메시지용 바이트 변환 (wide 문자 수와 narrow 바이트 수가 다르다)
	const AString narrowInput = StringConvert::ToAnsi(inputString.ToString());
	int length = narrowInput.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 = _pClient->EnqueueCmd<StaticMessage>();
	DynamicMessage& msg2 = _pClient->EnqueueCmd<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.msg_.Source, inputString.Source);
	StringUtilA::CopyUnsafe(msg2.Msg(), narrowInput.Source());

	// 다이나믹 패킷을 활용한 전송
	auto pPacket1 = dbg_new DynamicCmdPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto pMsg3 = pPacket1->Get<0>();
	auto pMsg4 = pPacket1->Get<1>();
	auto pMsg5 = pPacket1->Get<2>();
	StringUtilA::CopyUnsafe(pMsg3->Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(pMsg4->Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(pMsg5->Msg(), narrowInput.Source());
	_pClient->SendAsync(pPacket1);

	// 커맨드 버퍼를 활용한 전송 테스트
	PacketBufferPtr pBuffer = PacketBuffer::Create(_pClient->GetBufferAllocator());
	DynamicMessage& msg6 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg7 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg8 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	StringUtilA::CopyUnsafe(msg6.Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(msg7.Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(msg8.Msg(), narrowInput.Source());
	_pClient->SendAsync(pBuffer);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(length);
	StringUtilA::CopyUnsafe(pMsg9->cmd_.Msg(), narrowInput.Source());
	_pClient->SendAsync(pMsg9);
	
	// 스태틱 패킷 전송
	auto pPacket = dbg_new StaticCmdPacket<StaticMessage>();
	StaticMessage* pArg1 = pPacket->Get<0>();
	pArg1->msg_.SetString(inputString);
	auto wsaBuf = pPacket->GetWSABuf();
	_pClient->SendAsync(pPacket);

	_pClient->FlushSendBuffer();


	jc::CMessage legMsg = _pClient->EnqueueMsg();
	legMsg.SetMsgId(NET_TEST_MSG_1);
	legMsg.WriteU32(1234);
	legMsg.WriteString(_T("abcdefg"));

	jc::CMessage legMsg2 = _pClient->EnqueueMsg();
	legMsg2.SetMsgId(NET_TEST_MSG_2);
	legMsg2.WriteU32(5678);
	legMsg2.WriteString(_T("hijklmn"));

	_u32 aaa = legMsg.ReadU32();
	String bbb = legMsg.ReadString();

	aaa = legMsg2.ReadU32();
	bbb = legMsg2.ReadString();
	_pClient->FlushSendBuffer();
}
