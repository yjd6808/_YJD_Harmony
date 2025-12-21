/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 6:18:07 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "R_LOBBY.h"

#include <SteinsGate/Common/Cmd_LOBBY.h>
#include <SteinsGate/Client/Define_Popup.h>
#include <SteinsGate/Client/UI_ChannelSelect.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
void R_LOBBY::RECV_LOC_JoinLobbyAck(JCORE_UNUSED Session* _pSession, ICommand* _pCommand)
{
	LOC_JoinLobbyAck* pCmd = (LOC_JoinLobbyAck*)_pCommand;
	Core::Contents.PopupManager->CloseByTag(DEF_POPUP_AUTHENTICATION_LOBBY_BEGIN);
	Core::Contents.PopupManager->ShowNone(
		SG_TEXT_RAW("LOBBY_LOADING_CHANNEL_INFO"), false, Const::Timeout::LoadingChannel);
	Core::Contents.World->ReserveScene(SceneType::ChannelSelect);
	Core::Contents.UIManager->pChannelSelect_->SelectServer(pCmd->DefaultSelectedServer);
}
