/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthEventListener.h"

USING_NS_JC;
USING_NS_JNET;

void AuthEventListener::OnStarted() {
	_LogPlain_("서버가 시작되었습니다.");
	/*
	const auto spQueryResult = CoreGameDB_v->Query("select c_uid, c_id from t_account");
	const int iResultRowCount = spQueryResult->GetResultRowCount();
	for (int i = 0; i < iResultRowCount; i++) {
		_LogPlain_("%d", spQueryResult->GetInt(i, 0));
		_LogPlain_("%s", spQueryResult->GetString(i, 1).Source());
	}
	*/
}

void AuthEventListener::OnConnected(JNetwork::Session* connectedSession) {
}

void AuthEventListener::OnDisconnected(JNetwork::Session* disconnetedSession) {
}

void AuthEventListener::OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void AuthEventListener::OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) {
}

void AuthEventListener::OnStopped() {
	_LogPlain_("서버가 중지되었습니다.");
}
