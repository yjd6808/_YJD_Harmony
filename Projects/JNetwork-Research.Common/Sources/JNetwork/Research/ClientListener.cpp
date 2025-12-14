/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:49:33 PM
 * =====================
 *
 */


#include "Research.h"
#include <JNetwork/Research/ClientListener.h>
#include <JNetwork/Research/Command.h>

#include <JNetwork/Packet/Packet.h>
#include <JCore/Utils/Console.h>

USING_NS_JC;

NS_JNET_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
ClientListener::ClientListener(const JCore::String& _name)
: name_(_name)
{
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnConnected(Session* _pSession)
{
	Console::WriteLine("[%s] 연결되었습니다.", name_.Source());
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnDisconnected(Session* _pSession, Int32U _errorCode)
{
	Console::WriteLine("[%s] 연결이 종료되었습니다.", name_.Source());
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnSent(Session* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	CommandPacket* pSentPacket = dynamic_cast<CommandPacket*>(_pSentPacket);
	if (pSentPacket == nullptr)
	{
		DebugAssert(false);
	}
	Console::WriteLine("[%s] 커맨드 %d개를 송신했습니다.", name_.Source(), pSentPacket->GetCommandCount());
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnReceived(Session* _pSession, ICommand* _pCommand)
{
	if (_pCommand->GetId() == CMD_SATIC_MESSAGE)
		Console::WriteLine("[%s] 스태틱 메시지를 수신했습니다. : %s", name_.Source(), _pCommand->CastCommand<StaticMessage*>()->msg_.Source);
	else if (_pCommand->GetId() == CMD_DYNAMIC_MESSAGE)
	{
		DynamicMessage* pDynamicMessage = _pCommand->CastCommand<DynamicMessage*>();
		Console::WriteLine("[%s] 다이나믹 메시지를 수신했습니다. : %s(길이 : %d)", name_.Source(), pDynamicMessage->Msg(), pDynamicMessage->count_ - 1);	
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void ClientListener::OnConnectFailed(Session* _pSession, Int32U _errorMessage)
{
	Console::WriteLine("[%s] 서버 접속에 실패했습니다 (오류코드: %u)", name_.Source(), _errorMessage);
}

NS_JNET_END
