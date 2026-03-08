/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:49:33 PM
 * =====================
 *
 */


#include <jnetr_common/Core.h>
#include <jnetr_common/ClientListener.h>
#include <jnetr_common/Command.h>
#include <jnetr_common/Message.h>

#include <jnet/Packet/Packet.h>
#include <jnet/Packet/PacketReader.h>

#include <jc/Utils/Console.h>

USING_NS_JC;

NS_JNET_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
ClientListener::ClientListener(const jc::String& _name)
: name_(_name)
{
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnConnected(Session* _pSession)
{
	Console::WriteLine("[%s] 연결되었습니다.", name_.Source());
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnDisconnected(Session* _pSession, _u32 _errorCode)
{
	Console::WriteLine("[%s] 연결이 종료되었습니다.", name_.Source());
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnSent(Session* _pSession, IPacket* _pSentPacket, _u32l _sentBytes)
{
	PacketReader reader(_pSentPacket->GetWSABuf());
	PacketElementView view;

	while (reader.Next(view) == PacketReader::rrSuccess)
	{
		if (view.type_ == static_cast<_u8>(PacketType::Raw))
		{
			Console::WriteLine("[%s] RAW 데이터를 송신했습니다. (길이: %u)", name_.Source(), view.raw_.len);
		}
		else if (view.type_ == static_cast<_u8>(PacketType::Command))
		{
			Console::WriteLine("[%s] 커맨드 %d개를 송신했습니다.", name_.Source(), view.cmd_->GetId());
		}
		else if (view.type_ == static_cast<_u8>(PacketType::Message))
		{
			Console::WriteLine("[%s] CMessage를 송신했습니다. (메시지ID: %u)", name_.Source(), view.msg_.GetMsgId());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnReceivedCmd(Session* _pSession, ICommand* _pCommand)
{
	if (_pCommand->GetId() == CMD_SATIC_MESSAGE)
		Console::WriteLine("[%s] 스태틱 메시지를 수신했습니다. : %s", name_.Source(), _pCommand->CastCommand<StaticMessage*>()->msg_.Source);
	else if (_pCommand->GetId() == CMD_DYNAMIC_MESSAGE)
	{
		DynamicMessage* pDynamicMessage = _pCommand->CastCommand<DynamicMessage*>();
		Console::WriteLine("[%s] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", name_.Source(), pDynamicMessage->Msg(), pDynamicMessage->count_ - 1);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnReceivedMsg(Session* _pSession, jc::CMessage _msg)
{
 	if (_msg.GetMsgId() == NET_TEST_MSG_1)
 	{
		_u32 a = _msg.ReadU32();
		String str = _msg.ReadString();

		Console::WriteLine("[%s] CMessage를 수신했습니다. : %u, %s", name_.Source(), a, str.Source());
 	}
	else if (_msg.GetMsgId() == NET_TEST_MSG_2)
	{
		_u32 a = _msg.ReadU32();
		String str = _msg.ReadString();

		Console::WriteLine("[%s] CMessage를 수신했습니다. : %u, %s", name_.Source(), a, str.Source());
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnConnectFailed(Session* _pSession, _u32 _errorMessage)
{
	Console::WriteLine("[%s] 서버 접속에 실패했습니다 (오류코드: %u)", name_.Source(), _errorMessage);
}

NS_END
