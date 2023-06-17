/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerInterServerClientUdp.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

ListenerInterServerClientUdp::ListenerInterServerClientUdp() {
}

void ListenerInterServerClientUdp::OnReceived(SGSession* ssession, JNetwork::ICommand* cmd) {
	ListenerCommonClient::OnReceived(ssession, cmd);
}