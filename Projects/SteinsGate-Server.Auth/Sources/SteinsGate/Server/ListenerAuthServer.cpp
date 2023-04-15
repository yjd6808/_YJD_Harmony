/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerAuthServer.h"

USING_NS_JC;
USING_NS_JNET;

void ListenerAuthServer::OnStarted() {
	_LogInfo_("서버가 시작되었습니다.");
	/*
	const auto spQueryResult = CoreGameDB_v->Query("select c_uid, c_id from t_account");
	const int iResultRowCount = spQueryResult->GetResultRowCount();
	for (int i = 0; i < iResultRowCount; i++) {
		_LogPlain_("%d", spQueryResult->GetInt(i, 0));
		_LogPlain_("%s", spQueryResult->GetString(i, 1).Source());
	}
	*/
}

void ListenerAuthServer::OnConnected(JNetwork::Session* connectedSession) {
}

void ListenerAuthServer::OnDisconnected(JNetwork::Session* disconnetedSession) {
}

void ListenerAuthServer::OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void ListenerAuthServer::OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) {
}

void ListenerAuthServer::OnStopped() {
	_LogInfo_("서버가 중지되었습니다.");
}
