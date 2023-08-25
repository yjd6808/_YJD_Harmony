/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "C_LOBBY.h"

#include <SteinsGate/Client/Define_Popup.h>

#include <SteinsGate/Common/Cmd_CHAT.h>
#include <SteinsGate/Client/S_LOBBY.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


void C_LOBBY::OnConnected(Session* session) {
	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_LOBBY);
	Core::Contents.PopupManager->showNone(SG_TEXT_RAW("AUTHENTICATION_LOBBY_BEGIN"), DEF_POPUP_AUTHENTICATION_LOBBY_BEGIN);
	S_LOBBY::SEND_CLO_JoinLobby();
}

void C_LOBBY::OnConnectFailed(Session* session, Int32U errorCode) {
	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_LOBBY);
	Core::Contents.PopupManager->showOk(SG_TEXT_RAW_FMT_STD("CONNECT_LOBBY_FAILED_WITH_CODE", errorCode));
}

void C_LOBBY::OnDisconnected(Session* session) {
	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_LOBBY);
}
//
//const AccountData& accountData = Core::Net->getAuthenticationComponent()->getAccountData();
//S_AUTH::SEND_CAU_Login(accountData.Id.Source, accountData.Pass.Source);
//
//Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);
//Core::Contents.PopupManager->showNone(SG_TEXT_RAW("LOGIN_BEGIN"), DEF_POPUP_LOGIN_BEGIN);
//}
//
//void C_AUTH::OnConnectFailed(Session* session, Int32U errorCode) {
//	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);
//	Core::Contents.PopupManager->showOk(SG_TEXT_RAW_FMT_STD("CONNECT_AUTH_FAILED_WITH_CODE", errorCode));
//}
//
//void C_AUTH::OnDisconnected(Session* session) {
//	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);