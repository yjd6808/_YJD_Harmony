/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "C_AUTH.h"

#include <sg/Cmd_AUTHENTICATION.h>
#include <sgcl/S_AUTH.h>
#include <sgcl/Define_Popup.h>
#include <sgcl/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void C_AUTH::OnConnected(Session* _pSession)
{
	S_AUTH::SEND_CAU_Login();

	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_AUTH);
	sg::Contents.PopupManager->ShowNone(SG_LOCAL_RAW("LOGIN_BEGIN"), DEF_POPUP_LOGIN_BEGIN, false,
	                                      Const::Timeout::Login);
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_AUTH::OnConnectFailed(Session* _pSession, _u32 _errorCode)
{
	std::string msg = StringUtils::format(SG_LOCAL_RAW("CONNECT_SERVER_FAILED_WITH_MESSAGE"), 
		ServerType::Name[ServerType::Auth],Winsock::ErrorMessageUTF8(_errorCode).Source(), _errorCode);
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_AUTH);
	sg::Contents.PopupManager->ShowOk(msg);
}

//////////////////////////////////////////////////////////////////////////////////////////
void C_AUTH::OnDisconnected(Session* _pSession)
{
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_CONNECT_AUTH);
}
