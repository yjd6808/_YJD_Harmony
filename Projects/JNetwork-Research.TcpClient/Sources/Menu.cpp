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
#include <JNetwork/Research/TcpClientNetGroup.h>
#include <JNetwork/Research/Command.h>
#include <JNetwork/Research/Config.h>

USING_NS_JNET;
USING_NS_STD;
USING_NS_JC;

extern TcpClientNetGroup* pClientGroup;

static void SendMsg(TcpClient* client);

void ConnectLogin() {
	// pClientGroup->GetLogin()->ConnectAsync(ServerLoginTcpAddr);
	pClientGroup->GetLogin()->Connect(ServerLoginTcpAddr, 1000);
}

void ConnectChannel() {
	// pClientGroup->GetChannel()->ConnectAsync(ServerChannelTcpAddr);
	pClientGroup->GetLogin()->Connect(ServerChannelTcpAddr, 1000);
}

void ConnectGame() {
	// pClientGroup->GetGame()->ConnectAsync(ServerGameTcpddr);
	pClientGroup->GetLogin()->Connect(ServerGameTcpAddr, 1000);
}

void SendLogin() {
	SendMsg(pClientGroup->GetLogin());
}

void SendChannel() {
	SendMsg(pClientGroup->GetChannel());
}

void SendGame() {
	SendMsg(pClientGroup->GetGame());
}

void DisconnectLogin() {
	pClientGroup->GetLogin()->Disconnect();
}

void DisconnectChannel() {
	pClientGroup->GetChannel()->Disconnect();
}

void DisconnectGame() {
	pClientGroup->GetGame()->Disconnect();
}


static void SendMsg(TcpClient* client) {
	StaticString<512> s;
	cout << "보낼 메시지 입력: ";

	if (!(cin >> s.Source).good()) {
		cout << "제대로 입력해주세요.\n";
		return;
	}

	int length = s.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 =client->SendAlloc<StaticMessage>();
	DynamicMessage& msg2 = client->SendAlloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.Msg.Source, s.Source);
	StringUtil::CopyUnsafe(msg2.Msg, s.Source);

	// 다이나믹 패킷을 활용한 전송
	auto packet1 = dbg_new DynamicPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto msg3 = packet1->Get<0>();
	auto msg4 = packet1->Get<1>();
	auto msg5 = packet1->Get<2>();

	StringUtil::CopyUnsafe(msg3->Msg, s.Source);
	StringUtil::CopyUnsafe(msg4->Msg, s.Source);
	StringUtil::CopyUnsafe(msg5->Msg, s.Source);

	// 커맨드 버퍼를 활용한 전송 테스트
	CommandBufferPtr buffer = CommandBuffer::Create(client->GetBufferAllocator());
	DynamicMessage& msg6 = buffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg7 = buffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg8 = buffer->Alloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg6.Msg, s.Source);
	StringUtil::CopyUnsafe(msg7.Msg, s.Source);
	StringUtil::CopyUnsafe(msg8.Msg, s.Source);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// auto msg9 = dbg_new SinglePacket<DynamicMessage>(); assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto msg9 = dbg_new SinglePacket<DynamicMessage>(length);
	StringUtil::CopyUnsafe(msg9->Cmd.Msg, s.Source);
	

	client->SendAsync(msg9);
	client->SendAsync(buffer);
	client->SendAsync(packet1);
	client->FlushSendBuffer();
}