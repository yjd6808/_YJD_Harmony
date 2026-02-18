/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 1:12:34 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Packet/SendHelper.h>

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool SendHelperBase::IsValidInformation(Session* _pSender, SendStrategy _strategy)
{
	const TransportProtocol protocol = _pSender->Protocol();

	if (protocol == TransportProtocol::TCP)
	{
		if (_strategy == SendStrategy::SendToAsync)
		{
			_LogWarn_("TCP인데 UDP 송신전략을 사용할려고합니다.");
			return false;
		}
	}

	if (protocol == TransportProtocol::UDP)
	{
		if (_strategy == SendStrategy::SendAsync)
		{
			_LogWarn_("UDP인데 TCP 송신전략을 사용할려고합니다.");
			return false;
		}
	}

	return true;
}

NS_END
