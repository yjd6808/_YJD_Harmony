/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 6:18:07 PM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Net/Recv/R_LOBBY.h"

#include "sg/Cmd_LOBBY.h"

#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Game/Contents/UIManager.h"

#include "sgcl/Define/Define_Popup.h"
#include "sgcl/Game/UI_Implementation/UI_ChannelSelect.h"

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
}
