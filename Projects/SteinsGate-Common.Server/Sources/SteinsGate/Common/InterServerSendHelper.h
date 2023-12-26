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
struct InterServerSendHelperBase : JNetwork::SendHelperBase
{
	struct Information
	{
		JNetwork::Session* Sender;				// 패킷을 전송할 호스트
		JNetwork::SendStrategy Strategy;
		JNetwork::IPv4EndPoint Destination;
		int ToId;

		Information();
	};

	struct AutoFlush
	{
		~AutoFlush() { FlushSendBuffer(); }
	};

	static void InitSingleServerIds();
	static void InitSingleServerDestinations();

	static void FlushSendBuffer();
	static void SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, int toServerId = InvalidValue_v);
	static void SetInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, SingleServerType_t toServerType);
	static void SendEnd(JNetwork::IPacket* packet);

	static bool IsValidInformation(JNetwork::Session* sender, JNetwork::SendStrategy strategy, int toServerId);
	
	static int GetSenderId();

	inline static thread_local Information SendInformation;
	inline static /* readonly */ int SingleServerId[SingleServerType::Max];
	inline static /* readonly */ JNetwork::IPv4EndPoint SingleServerInterServerEP[SingleServerType::Max];
};

template <typename T, typename TCommand>
struct InterServerSending : JCore::NonCopyable
{
	InterServerSending(TCommand& cmd, JNetwork::IPacket* packet)
		: Cmd(cmd)
		, Packet(packet)
	{}

	~InterServerSending() {
		InterServerSendHelper<T>::SendEnd(Packet);

		if (Packet)
			Packet->Release();
	}

	TCommand& Cmd;
	JNetwork::IPacket* Packet;
};

template <typename T>
struct InterServerSendHelper : InterServerSendHelperBase
{
	template <typename TCommand>
	using TSending = InterServerSending<T, TCommand>;
	using THelper = InterServerSendHelper<T>;

	template <typename TCommand>
	static void InitCommand(TCommand& cmd) {
		if constexpr (IsInterServerRelayCommand_v<TCommand>) {
			cmd.From = GetSenderId();

			if (SendInformation.ToId == InvalidValue_v) {
				DebugAssertMsg(false, "누구에게 보낼지 설정되지 않았습니다.");
				return;
			}
			cmd.To = SendInformation.ToId;
		} else if constexpr (IsInterServerHostCommand_v<TCommand>){
			// 할거 없음
		} else {
			DebugAssert(false);
		}
	}


	template <typename TCommand>
	static TSending<TCommand> SendBegin(int count = 1) {
		DebugAssertMsg(SendInformation.Sender, "%s 샌더가 설정되어있지 않습니다.", TCommand::_Name());

		// InterServerCommand를 상속받지않은 커맨드를 전달하려는 경우를 막아야함.
		static_assert(IsInterServerCommand_v<TCommand>, "... TCommand is not InterServerCommand");

		if (SendInformation.Strategy == JNetwork::SendStrategy::SendAlloc) {
			TCommand& cmd = SendInformation.Sender->template SendAlloc<TCommand>(count);
			InitCommand(cmd);
			return TSending<TCommand>(cmd, nullptr);
		}

		auto pPacket = dbg_new JNetwork::SinglePacket<TCommand>(count);
		InitCommand(pPacket->Cmd);
		return TSending<TCommand>(pPacket->Cmd, pPacket);
	}

};


