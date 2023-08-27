/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:20:54 PM
 * =====================
 * 중앙 서버가 다른 서버로부터 수신한 커맨드 처리
 */



#include "Center.h"
#include "CenterCoreHeader.h"
#include "R_PING.h"

#include <SteinsGate/Common/CmdHost_PING.h>
#include <SteinsGate/Server/S_SETUP_IS.h>

USING_NS_JC;
USING_NS_JNET;

void R_PING::RECV_SCE_TimeSync(JNetwork::Session* session, JNetwork::ICommand* cmd) {
	SCE_TimeSync* pCmd = (SCE_TimeSync*)cmd;
	CenterSession* pSession = (CenterSession*)session;

	S_SETUP_IS::AutoFlush _;
	S_SETUP_IS::SetInformation(session, SendStrategy::SendAsync);
	S_SETUP_IS::SEND_CES_TimeSyncAck(*pCmd);
}




