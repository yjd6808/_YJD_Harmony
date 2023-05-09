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

#include <SteinsGate/Client/S_AUTH.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

void R_AUTH::RecvLoginAck(Session* session, ICommand* cmd) {
	CmdLoginAck* pCmd = (CmdLoginAck*)cmd;
	int a = 40;
}
