/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 1:12:34 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/SendHelper.h>

bool JNetwork::SendHelperBase::IsValidInformation(Session* sender, SendStrategy strategy) {
	const TransportProtocol eProtocol = sender->Protocol();

	if (eProtocol == TransportProtocol::TCP) {

		if (strategy == SendStrategy::SendToAsync) {
			_LogWarn_("TCP인데 UDP 송신전략을 사용할려고합니다.");
			return false;
		}
	}

	if (eProtocol == TransportProtocol::UDP) {

		if (strategy == SendStrategy::SendAsync) {
			_LogWarn_("UDP인데 TCP 송신전략을 사용할려고합니다.");
			return false;
		}
	}
	return true;
}



