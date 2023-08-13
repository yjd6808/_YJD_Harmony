/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 1:57:39 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "R_AUTH.h"

#include <SteinsGate/Common/AuthCmd.h>

#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/S_AUTH.h>


USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void R_AUTH::RecvLoginAck(Session* session, ICommand* cmd) {
	session->Disconnect();

	AUC_LoginAck* pCmd = (AUC_LoginAck*)cmd;
	CorePopupManager_v->closeByTag(POPUP_LOGIN);
	
	switch (pCmd->Result) {
	case LoginResult::LoginSuccess:
		CorePopupManager_v->showOk(SG_TEXT_RAW("CONNECT_LOBBY"));
		// CorePopupManager_v->showNone(SG_TEXT_RAW("CONNECT_LOBBY"), POPUP_LOBBY_WAIT);
		// TODO: 로비서버 접속
		break;
	case LoginResult::RegisterSuccess:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_REGISTER_SUCCESS"));
		break;
	case LoginResult::IdAlreadyExist:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_ID_ALREADY_EXIST"));
		break;
	case LoginResult::IdPasswordMismatch:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_ID_PASSWORD_MISMATCH"));
		break;
	case LoginResult::Banned:
		CorePopupManager_v->showOk(SG_TEXT_RAW_FMT_STD("LOGIN_RESULT_BANNED", pCmd->BanBeginDate.FormatMysqlTime().Source(), pCmd->BanEndDate.FormatMysqlTime().Source()));
		break;
	case LoginResult::Logined:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_LOGINED"));
		break;
	case LoginResult::QueryFailed:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_QUERY_FAILED"));
		break;
	default:
		CorePopupManager_v->showOk(SG_TEXT_RAW("LOGIN_RESULT_UNKNONW"));
		break;
	}
	
}
