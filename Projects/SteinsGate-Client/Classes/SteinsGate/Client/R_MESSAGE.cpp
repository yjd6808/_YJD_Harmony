/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:21:32 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "R_MESSAGE.h"

#include <SteinsGate/Common/Cmd_MESSAGE.h>

#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/AuthenticationComponent.h>

#include <SteinsGate/Client/Callback_POPUP_TIMEOUT.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void R_MESSAGE::RECV_SC_ClientText(Session* session, ICommand* cmd) {
	SC_ClientText* pCmd = (SC_ClientText*)cmd;
	Core::Contents.PopupManager->showOk(StringUtils::format("리모트 메시지\n%s", SG_TEXT_RAW(pCmd->Key.Source)));

}
