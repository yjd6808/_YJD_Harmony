/*
 * 작성자: 윤정도
 * 생성일: 4/22/2023 7:30:19 PM
 * =====================
 * 다양한 송신 방식을 하나로 통합하기 위함.
 * TODO 단일 세션을 대상으로만 동작하므로.. 복수 세션도 통합하여 전송가능하도록 전반적인 구조 및 기능 개선이 필요하다!!
 */


#pragma once

#include <jnet/Host/Session.h>
#include <jnet/Packet/Packet.h>

#include <jc/Pattern/NonCopyableh.h>

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

struct SendingBase
{
	SendingBase(IPacket* _pPacket)
		: Packet(_pPacket)
		, Sended(false)
	{
	}

	IPacket* Packet;
	bool Sended;
};

template <typename T, typename TCommand>
struct Sending : jc::NonCopyable, SendingBase
{
	Sending(TCommand& _command, IPacket* _pPacket)
		: SendingBase(_pPacket)
		, Cmd(_command)
	{
	}

	~Sending()
	{
		if (!Sended)
		{
			SendHelper<T>::SendEnd(Packet);
			Sended = true;
		}

		if (Packet)
			Packet->Release();
	}

	TCommand& Cmd;
};

struct SendHelperBase
{
	static bool IsValidInformation(Session* _pSender, SendStrategy _strategy);
};

template <typename T>    // 상속시 static 필드 공유를 선택적으로 설정가능하도록 만들기 위한 템플릿
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
		jc::MemoryPoolAbstractPtr MemPool;
	};

	struct AutoFlush
	{
		~AutoFlush()
		{
			FlushSendBuffer();
		}
	};

	template <typename TCommand>
	static TSending<TCommand> SendBegin()
	{
		return SendBegin<TCommand>(0);
	}

	template <typename TCommand>
	static TSending<TCommand> SendBegin(int _count)
	{
		jc_assert_msg(SendInformation.Sender, "샌더가 설정되어있지 않습니다.");

		if (SendInformation.Strategy == SendStrategy::SendAlloc)
		{
			return TSending<TCommand>(SendInformation.Sender->template SendAlloc<TCommand>(_count), nullptr);
		}

		auto pPacket = SinglePacket<TCommand>::Create(SendInformation.MemPool, _count);    // 해제는 소멸자에서함

		if (pPacket->cmd_.GetLength() >= 2500)
		{
			const char* pName = TCommand::_Name();
			(void)pName; // name only used for debugging
			jc_assert(false);
		}

		return TSending<TCommand>(pPacket->cmd_, pPacket);
	}

	static void FlushSendBuffer()
	{
		if (SendInformation.Sender == nullptr)
		{
			_LogError_("샌더 미할당");
			return;
		}

		SendInformation.Sender->FlushSendBuffer();
	}

	static void SetInformation(Session* _pSender, SendStrategy _strategy)
	{
		SendInformation.Sender = _pSender;
		SendInformation.Strategy = _strategy;
	}

	static void SetInformation(Session* _pSender, SendStrategy _strategy, const IPv4EndPoint& _destination)
	{
		SendInformation.Sender = _pSender;
		SendInformation.Strategy = _strategy;
		SendInformation.Destination = _destination;
	}

	static void SetMemoryPool(const jc::MemoryPoolAbstractPtr& _memPool)
	{
		SendInformation.MemPool = _memPool;
	}

	static bool SendEnd(IPacket* _pPacket)
	{
		if (SendInformation.Sender == nullptr)
		{
			_LogError_("센더가 설정되지 않았습니다.");
			return false;
		}

		if (SendInformation.Strategy == SendStrategy::None)
		{
			_LogError_("송신 전략이 설정되지 않았습니다.");
			return false;
		}

		switch (SendInformation.Strategy)
		{
		case SendStrategy::SendAsync:
			return SendInformation.Sender->SendAsync(_pPacket);
		case SendStrategy::SendToAsync:
			return SendInformation.Sender->SendToAsync(_pPacket, SendInformation.Destination);
		case SendStrategy::SendAlloc:
			// 할거 없음
			break;
		default:
			_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
		}

		return true;
	}

	static bool SendEndExplicit(SendingBase& _sending)
	{
		IPacket* pPacket = _sending.Packet;
		_sending.Sended = true;
		return SendEnd(pPacket);
	}

#ifdef SEND_HELPER_NONE_THREAD_LOCAL_INFORMATION
	inline static Information SendInformation;
#else
	inline static thread_local Information SendInformation;
#endif
};

NS_JNET_END
