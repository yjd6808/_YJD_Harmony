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

static void SendMsg(UdpClient* client, const IPv4EndPoint& target);

void ConnectLogin() {
	pClientGroup->Get()->SetRemoteEndpoint(ServerLoginUdpAddr);
}

void ConnectChannel() {
	pClientGroup->Get()->SetRemoteEndpoint(ServerChannelUdpAddr);
}

void ConnectGame() {
	pClientGroup->Get()->SetRemoteEndpoint(ServerGameUdpAddr);
}

void ConnectClientA() {
	pClientGroup->Get()->SetRemoteEndpoint(ClientUdpAAddr);
}

void SendLogin() {
	SendMsg(pClientGroup->Get(), ServerLoginUdpAddr);
}

void SendChannel() {
	SendMsg(pClientGroup->Get(), ServerChannelUdpAddr);
}

void SendGame() {
	SendMsg(pClientGroup->Get(), ServerGameUdpAddr);
}

void SendClientA() {
	SendMsg(pClientGroup->Get(), ClientUdpAAddr);
}

void BindRecvFrom() {
	pClientGroup->Get()->Bind(ClientUdpBAddr);
	pClientGroup->Get()->RecvFromAsync();
}

void Disconnect() {
	pClientGroup->Get()->Disconnect();
}


static void SendMsg(UdpClient* client, const IPv4EndPoint& target) {
	StaticString<512> s;
	cout << "보낼 메시지 입력: ";

	if (!(cin >> s.Source).good()) {
		cout << "제대로 입력해주세요.\n";
		return;
	}

	int length = s.LengthWithNull();

	// 클라이언트 송신 버퍼를 활용한 전송
	StaticMessage& msg1 = client->SendAlloc<StaticMessage>();
	DynamicMessage& msg2 = client->SendAlloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg1.Msg.Source, s.Source);
	StringUtil::CopyUnsafe(msg2.Msg(), s.Source);

	// 다이나믹 패킷을 활용한 전송
	auto packet1 = dbg_new DynamicPacket<DynamicMessage, DynamicMessage, DynamicMessage>(length, length, length);
	auto msg3 = packet1->Get<0>();
	auto msg4 = packet1->Get<1>();
	auto msg5 = packet1->Get<2>();

	StringUtil::CopyUnsafe(msg3->Msg(), s.Source);
	StringUtil::CopyUnsafe(msg4->Msg(), s.Source);
	StringUtil::CopyUnsafe(msg5->Msg(), s.Source);

	// 커맨드 버퍼를 활용한 전송 테스트
	CommandBufferPtr buffer = CommandBuffer::Create(client->GetBufferAllocator());
	DynamicMessage& msg6 = buffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg7 = buffer->Alloc<DynamicMessage>(length);
	DynamicMessage& msg8 = buffer->Alloc<DynamicMessage>(length);

	StringUtil::CopyUnsafe(msg6.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg7.Msg(), s.Source);
	StringUtil::CopyUnsafe(msg8.Msg(), s.Source);

	// 싱글 패킷 전송 (스태틱, 다이나믹 커맨드 아무거나 가능)
	// auto msg9 = dbg_new SinglePacket<DynamicMessage>(); assert 발사: 다이나믹 커맨드는 명시적으로 무조건 사이즈 전달
	auto msg9 = dbg_new SinglePacket<DynamicMessage>(length);
	StringUtil::CopyUnsafe(msg9->Cmd.Msg(), s.Source);


	client->SendToAsync(msg9, target);
	client->SendToAsync(buffer, target);
	client->SendToAsync(packet1, target);
	client->FlushSendBuffer();
}