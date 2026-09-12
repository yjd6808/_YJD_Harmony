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

#include "jc/Primitives/StringConvert.h"

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
void ConnectClientA()
{
	pClientGroup->Get()->SetRemoteEndpoint(IPv4EndPoint{ CLIENT_UDP_A_ADDR });
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
void SendClientA()
{
	SendMsg(pClientGroup->Get(), IPv4EndPoint{ CLIENT_UDP_A_ADDR });
}

//////////////////////////////////////////////////////////////////////////////////////////
void BindRecvFrom()
{
	pClientGroup->Get()->Bind(IPv4EndPoint{ CLIENT_UDP_B_ADDR });
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
#ifdef _UNICODE
	wcout << _T("보낼 메시지 입력: ");

	if (!(wcin >> s.Source).good())
	{
		wcout << _T("제대로 입력해주세요.\n");
		return;
	}
#else
	cout << "보낼 메시지 입력: ";

	if (!(cin >> s.Source).good())
	{
		cout << "제대로 입력해주세요.\n";
		return;
	}
#endif

	// dynamic narrow 메시지용 바이트 변환 (wide 문자 수와 narrow 바이트 수가 다르다)
	const AString narrowInput = StringConvert::ToAnsi(s.ToString());
	int length = narrowInput.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 = _pClient->EnqueueCmd<StaticMessage>();
	DynamicMessage& msg2 = _pClient->EnqueueCmd<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.msg_.Source, s.Source);
	StringUtilA::CopyUnsafe(msg2.Msg(), narrowInput.Source());

	// 다이나믹 패킷을 활용한 전송
	auto pPacket = dbg_new DynamicCmdPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto pMsg3 = pPacket->Get<0>();
	auto pMsg4 = pPacket->Get<1>();
	auto pMsg5 = pPacket->Get<2>();

	StringUtilA::CopyUnsafe(pMsg3->Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(pMsg4->Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(pMsg5->Msg(), narrowInput.Source());

	// 커맨드 버퍼를 활용한 전송 테스트
	PacketBufferPtr pBuffer = PacketBuffer::Create(_pClient->GetBufferAllocator());
	DynamicMessage& msg6 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg7 = pBuffer->EmplaceCmd<DynamicMessage>(length);
	DynamicMessage& msg8 = pBuffer->EmplaceCmd<DynamicMessage>(length);

	StringUtilA::CopyUnsafe(msg6.Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(msg7.Msg(), narrowInput.Source());
	StringUtilA::CopyUnsafe(msg8.Msg(), narrowInput.Source());

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(); assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto pMsg9 = dbg_new SingleCmdPacket<DynamicMessage>(length);
	StringUtilA::CopyUnsafe(pMsg9->cmd_.Msg(), narrowInput.Source());

	_pClient->SendToAsync(pMsg9, _target);
	_pClient->SendToAsync(pBuffer, _target);
	_pClient->SendToAsync(pPacket, _target);
	_pClient->FlushSendBuffer();
}
