/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "C_AUTH.h"

#include <SteinsGate/Common/Cmd_AUTH.h>
#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/Define_Popup.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


void C_AUTH::OnConnected(Session* session) {
	const AccountData& accountData = CorePlayer_v->accountData();
	S_AUTH::SendLogin(accountData.Id.Source, accountData.Pass.Source);

	CorePopupManager_v->closeByTag(DEF_POPUP_LOGIN_WAIT);
	CorePopupManager_v->showNone(SG_TEXT_RAW("LOGIN_BEGIN"), DEF_POPUP_LOGIN);
}

void C_AUTH::OnConnectFailed(Session* session, Int32U errorCode) {
	CorePopupManager_v->closeByTag(DEF_POPUP_LOGIN_WAIT);
	CorePopupManager_v->showOk(SG_TEXT_RAW_FMT_STD("CONNECT_AUTH_FAILED_WITH_CODE", errorCode));
}

void C_AUTH::OnDisconnected(Session* session) {
	CorePopupManager_v->closeByTag(DEF_POPUP_LOGIN_WAIT);
}