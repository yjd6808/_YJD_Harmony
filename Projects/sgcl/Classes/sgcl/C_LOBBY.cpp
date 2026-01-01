/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "C_LOBBY.h"

#include <sgcl/Define_Popup.h>

#include <sg/Cmd_CHAT.h>
#include <sgcl/S_LOBBY.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void C_LOBBY::OnConnected(Session* _pSession)
{
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_LOBBY);
	sg::Contents.PopupManager->ShowNone(
		SG_TEXT_RAW_FMT_STD("AUTHENTICATION_BEGIN", ServerType::Name[ServerType::Lobby]),
		DEF_POPUP_AUTHENTICATION_LOBBY_BEGIN, false, Const::Timeout::LobbyAuthentication);
	S_LOBBY::SEND_CLO_JoinLobby();
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_LOBBY::OnConnectFailed(Session* _pSession, Int32U _errorCode)
{
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_LOBBY);
	sg::Contents.PopupManager->ShowOk(SG_TEXT_RAW_FMT_STD("CONNECT_SERVER_FAILED_WITH_MESSAGE",
	                                                        ServerType::Name[ServerType::Lobby],
	                                                        Winsock::ErrorMessageUTF8(_errorCode).Source(),
	                                                        _errorCode));
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_LOBBY::OnDisconnected(Session* _pSession)
{
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_LOBBY);
}
