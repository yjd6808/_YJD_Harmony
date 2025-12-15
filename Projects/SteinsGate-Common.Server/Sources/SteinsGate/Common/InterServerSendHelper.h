/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:05:31 AM
 * =====================
 *
 */

#pragma once

#include <JCore/Container/Vector.h>
#include <JNetwork/Packet/SendHelper.h>

#include <SteinsGate/Common/InterServerCmdDefine.h>
#include <SteinsGate/Common/ServerEnum.h>

template <typename T>
struct InterServerSendHelper;

//////////////////////////////////////////////////////////////////////////////////////////
struct InterServerSendHelperBase : JNetwork::SendHelperBase
{
	struct Information
	{
		JNetwork::Session* sender_; // 패킷을 전송할 호스트
		JNetwork::SendStrategy strategy_;
		JNetwork::IPv4EndPoint destination_;
		int toId_;

		Information();
	};

	struct AutoFlush
	{
		~AutoFlush()
		{
			FlushSendBuffer();
		}
	};

	static void InitSingleServerIds();
	static void InitSingleServerDestinations();

	static void FlushSendBuffer();
	static void SetInformation(JNetwork::Session* _pSender, JNetwork::SendStrategy _strategy,
	                           int _toServerId = InvalidValue_v);
	static void SetInformation(JNetwork::Session* _pSender, JNetwork::SendStrategy _strategy,
	                           SingleServerType_t _toServerType);
	static void SendEnd(JNetwork::IPacket* _packet);

	static bool IsValidInformation(JNetwork::Session* _pSender, JNetwork::SendStrategy _strategy, int _toServerId);

	static int GetSenderId();

	inline static thread_local Information SendInformation;
	inline static /* readonly */ int SingleServerId[SingleServerType::Max];
	inline static /* readonly */ JNetwork::IPv4EndPoint SingleServerInterServerEP[SingleServerType::Max];
};

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T, typename TCommand>
struct InterServerSending : JCore::NonCopyable
{
	InterServerSending(TCommand& _cmd, JNetwork::IPacket* _pPacket)
	: cmd_(_cmd)
	, pPacket_(_pPacket)
	{
	}

	~InterServerSending()
	{
		InterServerSendHelper<T>::SendEnd(pPacket_);

		if (pPacket_)
			pPacket_->Release();
	}

	TCommand& cmd_;
	JNetwork::IPacket* pPacket_;
};


template <typename T>
struct InterServerSendHelper : InterServerSendHelperBase
{
	template <typename TCommand>
	using TSending = InterServerSending<T, TCommand>;
	using THelper = InterServerSendHelper<T>;

	template <typename TCommand>
	static void InitCommand(TCommand& _cmd)
	{
		if constexpr (IsInterServerRelayCommand_v<TCommand>)
		{
			_cmd.From = GetSenderId();

			if (SendInformation.toId_ == InvalidValue_v)
			{
				DebugAssertMsg(false, "누구에게 보낼지 설정되지 않았습니다.");
				return;
			}

			_cmd.To = SendInformation.toId_;
		}
		else if constexpr (IsInterServerHostCommand_v<TCommand>)
		{
			// 할거 없음
		}
		else
		{
			DebugAssert(false);
		}
	}

	template <typename TCommand>
	static TSending<TCommand> SendBegin(int _count = 1)
	{
		DebugAssertMsg(SendInformation.sender_, "%s 샌더가 설정되어있지 않습니다.", TCommand::_Name());

		// InterServerCommand를 상속받지않은 커맨드를 전달하려는 경우를 막아야함.
		static_assert(IsInterServerCommand_v<TCommand>, "... TCommand is not InterServerCommand");

		if (SendInformation.strategy_ == JNetwork::SendStrategy::SendAlloc)
		{
			TCommand& cmd = SendInformation.sender_->template SendAlloc<TCommand>(_count);
			InitCommand(cmd);
			return TSending<TCommand>(cmd, nullptr);
		}

		auto pPacket = dbg_new JNetwork::SinglePacket<TCommand>(_count);
		InitCommand(pPacket->cmd_);
		return TSending<TCommand>(pPacket->cmd_, pPacket);
	}
};
