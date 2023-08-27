/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "Center.h"
#include "CenterCoreHeader.h"
#include "R_MESSAGE.h"

#include <SteinsGate/Common/CmdHost_MESSAGE.h>

USING_NS_JC;
USING_NS_JNET;

void R_MESSAGE::RECV_SS_HostMessage(Session* session, ICommand* cmd) {
	SS_HostMessage* pCmd = (SS_HostMessage*)cmd;
	_LogInfo_(pCmd->Msg.Source);
}
