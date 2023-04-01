/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterEventListener.h"

USING_NS_JC;
USING_NS_JNET;

void CenterEventListener::OnStarted() {
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

void CenterEventListener::OnConnected(JNetwork::Session* connectedSession) {
}

void CenterEventListener::OnDisconnected(JNetwork::Session* disconnetedSession) {
}

void CenterEventListener::OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void CenterEventListener::OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) {
}

void CenterEventListener::OnStopped() {
	_LogPlain_("서버가 중지되었습니다.");
}
