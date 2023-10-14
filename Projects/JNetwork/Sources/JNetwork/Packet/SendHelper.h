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

enum class SendStrategy
{
	None,
	SendAlloc,
	SendAsync,
	SendToAsync
};

template <typename>
struct SendHelper;

template <typename T, typename TCommand>
struct Sending : JCore::NonCopyable
{
	Sending(TCommand& cmd, ISendPacket* packet)
		: Cmd(cmd)
		, Packet(packet)
	{}

	~Sending() { SendHelper<T>::SendEnd(Packet); }

	TCommand& Cmd;
	ISendPacket* Packet;
};

struct SendHelperBase
{
	static bool IsValidInformation(Session* sender, SendStrategy strategy);
};

template <typename T>	// 상속시 static 필드 공유를 선택적으로 설정가능하도록 만들기 위한 템플릿
struct SendHelper : SendHelperBase
{
	template <typename TCommand>
	using TSending = Sending<T, TCommand>;
	using THelper = SendHelper<T>;

	struct Information
	{
		Session* Sender = nullptr;
		SendStrategy Strategy = SendStrategy::SendAsync;
		IPv4EndPoint Destination;
	};

	struct AutoFlush
	{
		~AutoFlush() { FlushSendBuffer(); }
	};


	template <typename TCommand>
	static TSending<TCommand> SendBegin(int count = 1) {
		DebugAssertMsg(SendInformation.Sender, "샌더가 설정되어있지 않습니다.");

		if (SendInformation.Strategy == SendStrategy::SendAlloc) {
			return TSending<TCommand>(SendInformation.Sender->template SendAlloc<TCommand>(count), nullptr);
		}

		auto pPacket = dbg_new SinglePacket<TCommand>(count);
		return TSending<TCommand>(pPacket->Cmd, pPacket);
	}


	static void FlushSendBuffer() {
		if (SendInformation.Sender == nullptr) {
			_LogError_("샌더 미할당");
			return;
		}

		SendInformation.Sender->FlushSendBuffer();
	}

	static void SetInformation(Session* sender, SendStrategy strategy) {
		SendInformation.Sender = sender;
		SendInformation.Strategy = strategy;
	}

	static void SetInformation(Session* sender, SendStrategy strategy, const IPv4EndPoint& destination) {
		SendInformation.Sender = sender;
		SendInformation.Strategy = strategy;
		SendInformation.Destination = destination;
	}

	

	static void SendEnd(ISendPacket* packet) {
		if (SendInformation.Sender == nullptr) {
			_LogError_("센더가 설정되지 않았습니다.");
			return;
		}

		if (SendInformation.Strategy == SendStrategy::None) {
			_LogError_("송신 전략이 설정되지 않았습니다.");
			return;
		}

		switch (SendInformation.Strategy) {
		case SendStrategy::SendAsync:
			SendInformation.Sender->SendAsync(packet);
			break;
		case SendStrategy::SendToAsync:
			SendInformation.Sender->SendToAsync(packet, SendInformation.Destination);
			break;
		case SendStrategy::SendAlloc:
			// 할거 없음
			break;
		default:
			_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
		}
	}


#ifdef SEND_HELPER_NONE_THREAD_LOCAL_INFORMATION
	inline static Information SendInformation;
#else
	inline static thread_local Information SendInformation;
#endif

};



NS_JNET_END
