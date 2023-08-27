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

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void R_LOBBY::RECV_LOC_JoinLobbyAck(JCORE_UNUSED Session* session, ICommand* cmd) {
	LOC_JoinLobbyAck* pCmd = (LOC_JoinLobbyAck*)cmd;

	Core::Contents.PopupManager->closeByTag(DEF_POPUP_AUTHENTICATION_LOBBY_BEGIN);

	if (!pCmd->Success) {
		Core::Contents.PopupManager->showNone("d");
		return;
	}

	_LogInfo_("인증 성공");
}
