/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 1:57:39 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "R_AUTHENTICATION.h"

#include <SteinsGate/Common/Cmd_AUTHENTICATION.h>

#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/S_AUTH.h>
#include <SteinsGate/Client/AuthenticationComponent.h>

#include <SteinsGate/Client/Callback_POPUP_TIMEOUT.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_AUTHENTICATION::RECV_AUC_LoginAck(Session* _pSession, ICommand* _pCommand)
{
	_pSession->Disconnect();

	AUC_LoginAck* pLoginAck = (AUC_LoginAck*)_pCommand;
	Core::Contents.PopupManager->CloseByTag(DEF_POPUP_LOGIN_BEGIN);

	switch (pLoginAck->result_)
	{
	case LoginResult::LoginSuccess:
		{
			Core::Contents.PopupManager->ShowNone(
				SG_TEXT_RAW_FMT_STD("CONNECT_SERVER", ServerType::Name[ServerType::Lobby]), DEF_POPUP_CONNECT_LOBBY,
				false, Const::Timeout::LobbyConnection);

			if (!Core::Net->ConnectLobbyTcp())
			{
				Core::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_LOBBY);
				Core::Contents.PopupManager->ShowOk(
					SG_TEXT_RAW_FMT_STD("CONNECT_LOBBY_FAILED_WITH_CODE", Winsock::LastError()));
				break;
			}

			AuthenticationComponent* pAuthenticationComponent = Core::Net->GetAuthenticationComponent();
			pAuthenticationComponent->SetState(AuthenticationState::LobbyWait);
			pAuthenticationComponent->SetSerial(pLoginAck->serial_);
			pAuthenticationComponent->SetLastServer(pLoginAck->lastServer_);
			break;
		}
	case LoginResult::RegisterSuccess:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_REGISTER_SUCCESS"));
		break;
	case LoginResult::IdAlreadyExist:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_ID_ALREADY_EXIST"));
		break;
	case LoginResult::IdPasswordMismatch:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_ID_PASSWORD_MISMATCH"));
		break;
	case LoginResult::Banned:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW_FMT_STD("LOGIN_RESULT_BANNED",
		                                                        pLoginAck->banBeginDate_.FormatMysqlTime().Source(),
		                                                        pLoginAck->banEndDate_.FormatMysqlTime().Source()));
		break;
	case LoginResult::Logined:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_LOGINED"));
		break;
	case LoginResult::QueryFailed:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_QUERY_FAILED"));
		break;
	default:
		Core::Contents.PopupManager->ShowOk(SG_TEXT_RAW("LOGIN_RESULT_UNKNONW"));
		break;
	}
}
