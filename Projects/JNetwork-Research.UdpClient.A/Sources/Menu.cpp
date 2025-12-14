/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 10:32:50 PM
 * =====================
 *
 */


#pragma once

#include "Menu.h"

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/Research/UdpClientNetGroup.h>
#include <JNetwork/Research/Command.h>
#include <JNetwork/Research/Config.h>

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

extern UdpClientNetGroup* pClientGroup;

static void SendMsg(UdpClient* _pClient, const IPv4EndPoint& _target);

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectLogin()
{
	pClientGroup->Get()->SetRemoteEndpoint(ServerLoginUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectChannel()
{
	pClientGroup->Get()->SetRemoteEndpoint(ServerChannelUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectGame()
{
	pClientGroup->Get()->SetRemoteEndpoint(ServerGameUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConnectClientB()
{
	pClientGroup->Get()->SetRemoteEndpoint(ClientUdpBAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendLogin()
{
	SendMsg(pClientGroup->Get(), ServerLoginUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendChannel()
{
	SendMsg(pClientGroup->Get(), ServerChannelUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendGame()
{
	SendMsg(pClientGroup->Get(), ServerGameUdpAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SendClientB()
{
	SendMsg(pClientGroup->Get(), ClientUdpBAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void BindRecvFrom()
{
	pClientGroup->Get()->Bind(ClientUdpAAddr);
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
	StaticMessage& msg1 = _pClient->SendAlloc<StaticMessage>();
	DynamicMessage& msg2 = _pClient->SendAlloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.msg_.Source, s.Source);
	StringUtil::CopyUnsafe(msg2.Msg(), s.Source);

	// 다이나믹 패킷을 활용한 전송
	auto pPacket = dbg_new DynamicPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto pMsg3 = pPacket->Get<0>();
	auto pMsg4 = pPacket->Get<1>();
	auto pMsg5 = pPacket->Get<2>();

	StringUtil::CopyUnsafe(pMsg3->Msg(), s.Source);
	StringUtil::CopyUnsafe(pMsg4->Msg(), s.Source);
	StringUtil::CopyUnsafe(pMsg5->Msg(), s.Source);

	// 커맨드 버퍼를 활용한 전송 테스트
	CommandBufferPtr pBuffer = CommandBuffer::Create(_pClient->GetBufferAllocator());
	DynamicMessage& msg6 = pBuffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg7 = pBuffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg8 = pBuffer->Alloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg6.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg7.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg8.Msg(), s.Source);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// auto pMsg9 = dbg_new SinglePacket<DynamicMessage>(); assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto pMsg9 = dbg_new SinglePacket<DynamicMessage>(length);
	StringUtil::CopyUnsafe(pMsg9->cmd_.Msg(), s.Source);

	_pClient->SendToAsync(pMsg9, _target);
	_pClient->SendToAsync(pBuffer, _target);
	_pClient->SendToAsync(pPacket, _target);
	_pClient->FlushSendBuffer();
}
