/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "ListenerCenterServer.h"

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/ListenerHelperCommon.h>

#include <SteinsGate/Server/R_SETUP.h>
#include <SteinsGate/Server/S_SETUP_IS.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ListenerCenterServer::ListenerCenterServer(CenterServer* _pServer, SGCommandParser* _pParser)
	: ListenerServerCommon(_pParser)
	, centerTcp_(_pServer)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnStarted()
{
	ListenerServerCommon::OnStarted();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnConnected(Session* _pConnectedSession)
{
	ListenerServerCommon::OnConnected(_pConnectedSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnDisconnected(Session* _pDisconnectedSession, Int32U _errorCode)
{
	ListenerServerCommon::OnDisconnected(_pDisconnectedSession, _errorCode);
	CenterSession* pSession = (CenterSession*)_pDisconnectedSession;

	if (pSession->IsValid())
	{
		Core::Server->RemoveSession(pSession);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnSent(Session* _pSender, IPacket* _pSentPacket, Int32UL _sentBytes)
{
	ListenerServerCommon::OnSent(_pSender, _pSentPacket, _sentBytes);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket)
{
	ms_tlsSenderMap.Clear();

	_pRecvPacket->ForEach(
		[this, _pSession](ICommand* _pCmd)
		{
			if (!ListenerHelperCommon::IsInterServerCommand(_pCmd))
			{
				_LogWarn_("인터서버 커맨드가 아닌 커맨드를 수신했습니다. (%d)", _pCmd->GetId());
				return;
			}

			if (ListenerHelperCommon::IsRelayCommand(_pCmd))
			{
				// 릴레이 커맨드인 경우
				RelayCommandBase* pInterServerCmd;

				if (_pCmd->GetType() == InterServerCmdType::RelayStatic)
				{
					pInterServerCmd = static_cast<RelayStaticCommand*>(_pCmd);
				}
				else
				{
					pInterServerCmd = static_cast<RelayDynamicCommand*>(_pCmd);
				}

				InterServerRecvHelper::LastFromId = pInterServerCmd->From;

				// 클라(나)가 수신처인 경우 그대로 처리
				if (pInterServerCmd->To == Core::ServerProcessInfo->serverId_)
				{
					ListenerServerCommon::OnReceived(_pSession, _pCmd);
				}
				else
				{
					RelayCommand(_pCmd, pInterServerCmd);
				}
			}
			else if (ListenerHelperCommon::IsHostCommand(_pCmd))
			{
				// 호스트 커맨드인 경우
				ListenerServerCommon::OnReceived(_pSession, _pCmd);
			}
			else
			{
				DebugAssertMsg(false, "유효하지 않은 타입의 커맨드를 수신했습니다. (%d)", _pCmd->GetId());
			}
		});

	// 보통 1개만 있겟지..?
	ms_tlsSenderMap.ForEach(
		[](Session* _pSessionInner)
		{
			_pSessionInner->FlushSendBuffer();
		});
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::OnStopped()
{
	ListenerServerCommon::OnStopped();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ListenerCenterServer::RelayCommand(ICommand* _pCmd, RelayCommandBase* _pRelayCmd)
{
	Session* pSender = Core::Server->GetCenterSession(_pRelayCmd->To);
	pSender->SendAlloc(_pCmd);
	ms_tlsSenderMap.Insert(pSender);
}
