/*
 * 작성자: 윤정도
 * 생성일: 4/22/2023 7:30:19 PM
 * =====================
 * 다양한 송신 방식을 하나로 통합하기 위함.
 */


#pragma once

#include <JNetwork/Host/Session.h>

#include <JCore/Pattern/NonCopyableh.h>

NS_JNET_BEGIN

enum SendStrategy
{
	eSendAlloc,
	eSendAsync,
	eSendToAsync,
	eSendToAsyncEcho
};

struct SendHelper;

template <typename TCommand>
struct Sending : JCore::NonCopyableButMovable
{
	Sending(TCommand& cmd, ISendPacket* packet)
		: Cmd(cmd)
		, Packet(packet)
	{}

	~Sending() { SendHelper::SendEnd(Packet); }

	TCommand& Cmd;
	ISendPacket* Packet;
};

struct SendHelper
{
	template <typename TCommand>
	static Sending<TCommand> SendBegin(int count = 1) {
		if (SendInformation.Strategy == eSendAlloc) {
			return Sending<TCommand>(SendInformation.Sender->SendAlloc<TCommand>(count), nullptr);
		}

		auto pPacket = dbg_new SinglePacket<TCommand>(count);
		return Sending<TCommand>(pPacket->Cmd, pPacket);
	}

	static void SendEnd(ISendPacket* packet);

	static void FlushSendBuffer();
	static void SetInformation(Session* sender);
	static void SetInformation(Session* sender, SendStrategy strategy);
	static void SetInformation(Session* sender, SendStrategy strategy, const IPv4EndPoint& destination);
private:
	struct Information
	{
		Session* Sender = nullptr;
		SendStrategy Strategy = eSendAsync;
		IPv4EndPoint Destination;
	};


public:
	inline static thread_local Information SendInformation;
};



NS_JNET_END
