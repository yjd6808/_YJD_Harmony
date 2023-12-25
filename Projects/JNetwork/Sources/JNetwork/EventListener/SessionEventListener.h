﻿/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/Declspec.h>

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

struct ICommand;
struct ISendPacket;
struct IRecvPacket;

class Session;
class JCORE_NOVTABLE SessionEventListener
{
public:
	SessionEventListener() = default;
	virtual ~SessionEventListener() = default;

	virtual void OnConnected(Session* session) {}
	virtual void OnDisconnected(Session* session) {}
	virtual void OnSent(Session* session, ISendPacket* sentPacket, Int32UL sentBytes) {}
	virtual void OnReceivedRaw(Session* session, Byte* data, int len) {}
	virtual void OnReceived(Session* session, ICommand* recvCmd) {}
	virtual void OnReceived(Session* session, IRecvPacket* recvPacket) {}
};

NS_JNET_END