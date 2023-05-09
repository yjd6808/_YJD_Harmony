/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerInterServerClient.h"

#include <SteinsGate/Common/InterServerCmd.h>

ListenerInterServerClient::ListenerInterServerClient() {
}

void ListenerInterServerClient::OnReceived(SGSession* ssession, JNetwork::ICommand* cmd) {

}