/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 5:21:32 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "R_MESSAGE.h"

#include <sg/Cmd_MESSAGE.h>

#include <sgcl/Define_Popup.h>
#include <sgcl/S_AUTH.h>
#include <sgcl/AuthenticationComponent.h>

#include <sgcl/Callback_POPUP_TIMEOUT.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_MESSAGE::RECV_SC_ClientText(Session* _pSession, ICommand* _pCmd)
{
	SC_ClientText* pCmd = (SC_ClientText*)_pCmd;
	sg::Contents.PopupManager->ShowOk(StringUtils::format("리모트 메시지\n%s", SG_TEXT_RAW(pCmd->Key.Source)));
}
