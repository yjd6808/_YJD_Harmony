/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 1:57:39 PM
 * =====================
 *
 */


#include "Core.h"
#include "R_AUTHENTICATION.h"

#include <sg/Cmd_AUTHENTICATION.h>

#include <sgcl/Define_Popup.h>
#include <sgcl/S_AUTH.h>
#include <sgcl/Contents.h>
#include <sgcl/_Net/NetCore.h>
#include <sgcl/AuthenticationComponent.h>

#include <sgcl/Callback_POPUP_TIMEOUT.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_AUC_LoginAck(Session* _pSession, ICommand* _pCommand)
{
	_pSession->Disconnect();

	AUC_LoginAck* pLoginAck = (AUC_LoginAck*)_pCommand;
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_LOGIN_BEGIN);

	switch (pLoginAck->result_)
	{
	case LoginResult::LoginSuccess:
		{
			std::string msg1 = StringUtils::format(SG_LOCAL_RAW("CONNECT_SERVER"), ServerType::Name[ServerType::Lobby]);
			sg::Contents.PopupManager->ShowNone(msg1, DEF_POPUP_CONNECT_LOBBY, false, Const::Timeout::LobbyConnection);

			if (!g_cNet.ConnectLobbyTcp())
			{
				std::string msg2 = StringUtils::format(SG_LOCAL_RAW("CONNECT_LOBBY_FAILED_WITH_CODE"), Winsock::LastError());
				sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_LOBBY);
				sg::Contents.PopupManager->ShowOk(msg2);
				break;
			}

			g_cNet.authentication_.SetState(AuthenticationState::LobbyWait);
			g_cNet.authentication_.SetSerial(pLoginAck->serial_);
			g_cNet.authentication_.SetLastServer(pLoginAck->lastServer_);
			break;
		}
	case LoginResult::RegisterSuccess:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_REGISTER_SUCCESS"));
		break;
	case LoginResult::IdAlreadyExist:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_ID_ALREADY_EXIST"));
		break;
	case LoginResult::IdPasswordMismatch:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_ID_PASSWORD_MISMATCH"));
		break;
	case LoginResult::Banned:
		{
			std::string msg = StringUtils::format(SG_LOCAL_RAW("LOGIN_RESULT_BANNED"),
				pLoginAck->banBeginDate_.FormatMysqlTime().Source(),
				pLoginAck->banEndDate_.FormatMysqlTime().Source());
			sg::Contents.PopupManager->ShowOk(msg);
		}
		break;
	case LoginResult::Logined:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_LOGINED"));
		break;
	case LoginResult::QueryFailed:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_QUERY_FAILED"));
		break;
	default:
		sg::Contents.PopupManager->ShowOk(SG_LOCAL_RAW("LOGIN_RESULT_UNKNONW"));
		break;
	}
}
