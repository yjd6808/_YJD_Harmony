/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IPEndPoint.h>

#include <JCore/Primitives/StaticString.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Exception.h>
#include <JCore/Ascii.h>

#include <JNetwork/ByteOrder.h>

NS_JNET_BEGIN

USING_NS_JC;

/*=====================================================================================
							IPEndPoint
=======================================================================================*/


/*=====================================================================================
							IPv4EndPoint
=======================================================================================*/

IPEndPoint::~IPEndPoint()
{
}

IPv4EndPoint::IPv4EndPoint() {
	m_Address = IPv4Address::Any();
	m_uiPort = 0;
}

IPv4EndPoint::IPv4EndPoint(const IPv4Address& ipAddr, const Int16U port) {
	m_Address = ipAddr;
	m_uiPort = port;
}

IPv4EndPoint::IPv4EndPoint(const char* endPointAddrString) {
	*this = Parse(endPointAddrString);
}

IPv4EndPoint::IPv4EndPoint(const SOCKADDR_IN& addr) {
	m_Address = ByteOrder::NetworkToHost(addr.sin_addr.S_un.S_addr);
	m_uiPort = ByteOrder::NetworkToHost(addr.sin_port);
}

IPv4EndPoint::IPv4EndPoint(const JCore::String& endPointAddrString) {
	*this = Parse(endPointAddrString.Source());
}

String IPv4EndPoint::ToString() const {
	String ret = GetAddress().ToString() + ':';
	ret += m_uiPort;
	return ret;
}

IPv4EndPoint IPv4EndPoint::Parse(const char* endPointAddr) {
	constexpr int EndPointLen = IPv4Len_v + 6; // +6 : 포트 최대 5자리 + 문자 ':'를 포함한 길이
	using IPv4EndPointString = StaticString<EndPointLen>;

	const int iLen = StringUtil::Length(endPointAddr);

	if (iLen > EndPointLen) {
		throw InvalidArgumentException("올바르지 않은 EndPoint 형식입니다. 문자열 길이가 최대로 가능한 EndPoint 길이를 초과합니다.");
	}

	IPv4EndPointString ep{};
	ep.CopyFrom(endPointAddr);
	const int idx = ep.Find(":");

	if (idx == -1) {
		throw InvalidArgumentException("올바르지 않은 EndPoint 형식입니다. 구분자 ':'를 찾지 못했습니다.");
	}

	IPv4EndPointString ipAddrString{};
	ipAddrString.CopyFrom(0, idx - 1, const_cast<char*>(ep.Source));

	IPv4EndPointString portString{};
	portString.CopyFrom(idx + 1, ep.Length() - 1, const_cast<char*>(ep.Source));
	const int iPortStrLen = portString.Length();

	for (int i = 0; i < iPortStrLen; i++) {
		if (!IsNumeric(portString[i])) {
			throw InvalidArgumentException("올바르지 않은 EndPoint 형식입니다. 잘못된 문자가 포함되어있습니다.");
		}
	}

if (const int iPort = atoi(portString.Source); iPort < 0 || iPort > 0xffff) {
		throw InvalidArgumentException("올바르지 않은 EndPoint 형식입니다. 포트번호는 0이상 65535이하만 가능합니다.");
	}

	return IPv4EndPoint(ipAddrString.Source, static_cast<short>(atoi(portString.Source)));
}

IPv4EndPoint IPv4EndPoint::Parse(const JCore::String& endPointAddr) {
	return Parse(endPointAddr.Source());
}


NS_JNET_END
