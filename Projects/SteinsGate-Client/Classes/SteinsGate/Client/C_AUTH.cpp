/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "C_AUTH.h"

#include <SteinsGate/Common/Cmd_AUTHENTICATION.h>
#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


void C_AUTH::OnConnected(Session* session) {
	S_AUTH::SEND_CAU_Login();

	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);
	Core::Contents.PopupManager->showNone(SG_TEXT_RAW("LOGIN_BEGIN"), DEF_POPUP_LOGIN_BEGIN, false, Const::Timeout::Login);
}

void C_AUTH::OnConnectFailed(Session* session, Int32U errorCode) {
	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);
	Core::Contents.PopupManager->showOk(SG_TEXT_RAW_FMT_STD("CONNECT_SERVER_FAILED_WITH_MESSAGE", ServerType::Name[ServerType::Auth], Winsock::ErrorMessage(errorCode).Source(), errorCode));
}

void C_AUTH::OnDisconnected(Session* session) {
	Core::Contents.PopupManager->closeByTag(DEF_POPUP_CONNECT_AUTH);
}