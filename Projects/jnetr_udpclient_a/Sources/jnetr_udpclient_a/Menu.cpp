/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 10:32:50 PM
 * =====================
 *
 */


#pragma once

#include "jnetr_common/PCH/Core.h"
#include "jnetr_common/UdpClientNetGroup.h"
#include "jnetr_common/Command.h"
#include "jnetr_common/Config.h"

#include "Menu.h"

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

extern UdpClientNetGroup* pClientGroup;

static void SendMsg(UdpClient* _pClient, const IPv4EndPoint& _target);

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectLogin()
{
	pClientGroup->Get()->SetRemoteEndpoint(IPv4EndPoint{ SERVER_LOGIN_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectChannel()
{
	pClientGroup->Get()->SetRemoteEndpoint(IPv4EndPoint{ SERVER_CHANNEL_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectGame()
{
	pClientGroup->Get()->SetRemoteEndpoint(IPv4EndPoint{ SERVER_GAME_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectClientB()
{
	pClientGroup->Get()->SetRemoteEndpoint(IPv4EndPoint{ CLIENT_UDP_B_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendLogin()
{
	SendMsg(pClientGroup->Get(), IPv4EndPoint{ SERVER_LOGIN_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendChannel()
{
	SendMsg(pClientGroup->Get(), IPv4EndPoint{ SERVER_CHANNEL_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendGame()
{
	SendMsg(pClientGroup->Get(), IPv4EndPoint{ SERVER_GAME_UDP_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendClientB()
{
	SendMsg(pClientGroup->Get(), IPv4EndPoint{ CLIENT_UDP_B_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void BindRecvFrom()
{
	pClientGroup->Get()->Bind(IPv4EndPoint{ CLIENT_UDP_A_ADDR });
	pClientGroup->Get()->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Disconnect()
{
	pClientGroup->Get()->Disconnect();
}


static void SendMsg(UdpClient* _pClient, const IPv4EndPoint& _target)
{
	StaticString<512> s;
	cout << "보낼 메시지 입력: ";

	if (!(cin >> s.Source).good())
	{
		cout << "제대로 입력해주세요.\n";
		return;
	}

	int length = s.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 = _pClient->EnqueueCmd<StaticMessage>();
	DynamicMessage& msg2 = _pClient->EnqueueCmd<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.msg_.Source, s.Source);
	StringUtil::CopyUnsafe(msg2.Msg(), s.Source);

	// 다이나믹 패킷을 활용한 전송
	auto pPacket = dbg_new DynamicCmdPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto pMsg3 = pPacket->Get<0>();
	auto pMsg4 = pPacket->Get<1>();
	auto pMsg5 = pPacket->Get<2>();

	StringUtil::CopyUnsafe(pMsg3->Msg(), s.Source);
	StringUtil::CopyUnsafe(pMsg4->Msg(), s.Source);
	StringUtil::CopyUnsafe(pMsg5->Msg(), s.Source);

	// 커맨드 버퍼를 활용한 전송 테스트
	PacketBufferPtr pBuffer = PacketBuffer::Create(_pClient->GetBufferAllocator());
	DynamicMessage& msg6 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg7 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg8 = pBuffer->EmplaceCmd<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg6.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg7.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg8.Msg(), s.Source);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(); assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(length);
	StringUtil::CopyUnsafe(pMsg9->cmd_.Msg(), s.Source);

	_pClient->SendToAsync(pMsg9, _target);
	_pClient->SendToAsync(pBuffer, _target);
	_pClient->SendToAsync(pPacket, _target);
	_pClient->FlushSendBuffer();
}
