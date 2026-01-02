/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 6:18:07 PM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "R_LOBBY.h"

#include <sg/Cmd_LOBBY.h>

#include <sgcl/WorldScene.h>
#include <sgcl/UIManager.h>

#include <sgcl/Define_Popup.h>
#include <sgcl/UI_ChannelSelect.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_LOBBY::RECV_LOC_JoinLobbyAck(UNUSED Session* _pSession, ICommand* _pCommand)
{
	LOC_JoinLobbyAck* pCmd = (LOC_JoinLobbyAck*)_pCommand;
	sg::Contents.PopupManager->CloseByTag(DEF_POPUP_AUTHENTICATION_LOBBY_BEGIN);
	sg::Contents.PopupManager->ShowNone(
		SG_LOCAL_RAW("LOBBY_LOADING_CHANNEL_INFO"), false, Const::Timeout::LoadingChannel);
	g_cWorld.ReserveScene(SceneType::ChannelSelect);
	g_cUIMgr.GetUI_ChannelSelect().SelectServer(pCmd->DefaultSelectedServer);
}
