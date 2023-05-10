/*
 * 작성자: 윤정도
 * 생성일: 5/10/2023 3:20:39 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "C_AUTH.h"

#include <SteinsGate/Common/AuthCmd.h>

#include <SteinsGate/Client/S_AUTH.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


void C_AUTH::OnConnected(Session* session) {
	const AccountData* pAccountData = CorePlayer_v->accountData();
	S_AUTH::SendLogin(pAccountData->Name.Source, pAccountData->Pass.Source);
}

void C_AUTH::OnConnectFailed(Session* session, Int32U errorCode) {
	CorePopupManager_v->showOk(StringUtils::format("인증 서버 접속에 실패했습니다. (%d)", errorCode));
}

void C_AUTH::OnDisconnected(Session* session) {
	
}