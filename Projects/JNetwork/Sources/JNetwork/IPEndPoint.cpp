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

IPv4EndPoint IPv4EndPoint::Invalid = []() {
	SOCKADDR_IN addr;
	Memory::Set(&addr, sizeof(SOCKADDR_IN), 0);
	return IPv4EndPoint(addr);
}();

IPv4EndPoint::IPv4EndPoint() : InternetAddr() {
	Memory::Set(&InternetAddr, sizeof(SOCKADDR_IN), 0);
	InternetAddr.sin_family = AF_INET;
}

IPv4EndPoint::IPv4EndPoint(const IPv4EndPoint& other) : InternetAddr() {
	this->operator=(other);
}

IPv4EndPoint::IPv4EndPoint(const SOCKADDR_IN& other) : InternetAddr() {
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = other.sin_addr.s_addr;
	InternetAddr.sin_port = other.sin_port;
}

IPv4EndPoint::IPv4EndPoint(const char* endPointString) {
	this->operator=(Parse(endPointString));
}

IPv4EndPoint::IPv4EndPoint(const JCore::String& endPointString) {
	this->operator=(Parse(endPointString.Source()));
}

IPv4EndPoint::IPv4EndPoint(IPv4Address addr, Int16U port) : InternetAddr() {
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = ByteOrder::HostToNetwork(addr.GetAddress());
	InternetAddr.sin_port = ByteOrder::HostToNetwork(port);
}

bool IPv4EndPoint::IsValidRemoteEndPoint() const {
	if (InternetAddr.sin_family == AF_INET &&
		InternetAddr.sin_addr.s_addr != ADDR_ANY &&
		InternetAddr.sin_port != 0) {
		return true;
	}

	return false;
}

bool IPv4EndPoint::IsValidEndPoint() const {
	if (InternetAddr.sin_family == AF_INET) {
		return true;
	}
	return false;
}

IPv4EndPoint& IPv4EndPoint::operator=(const IPv4EndPoint& other) {
	InternetAddr.sin_family = other.InternetAddr.sin_family;
	InternetAddr.sin_port = other.InternetAddr.sin_port;
	InternetAddr.sin_addr.s_addr = other.InternetAddr.sin_addr.s_addr;
	return *this;
}

bool IPv4EndPoint::operator==(const IPv4EndPoint& other) const {
	if (InternetAddr.sin_family == other.InternetAddr.sin_family &&
		InternetAddr.sin_port == other.InternetAddr.sin_port &&
		InternetAddr.sin_addr.s_addr == other.InternetAddr.sin_addr.s_addr) {
		return true;
	}

	return false;
}

InternetProtocol IPv4EndPoint::GetProtocol() const {
	return InternetProtocol::IPv4;
}

JCore::String IPv4EndPoint::ToString() const {
	String ret = GetAddress().ToString() + ':';
	ret += GetPort();
	return ret;
}

IPv4Address IPv4EndPoint::GetAddress() const {
	return ByteOrder::NetworkToHost(InternetAddr.sin_addr.s_addr);
}

Int16U IPv4EndPoint::GetPort() const {
	return ByteOrder::NetworkToHost(InternetAddr.sin_port);
}

IPv4EndPoint IPv4EndPoint::Parse(const char* endPointAddr) {
	static constexpr int EndPointLen = IPv4Len_v + 6; // +6 : 포트 최대 5자리 + 문자 ':'를 포함한 길이
	using IPv4EndPointString = StaticString<EndPointLen>;

	const int iLen = StringUtil::Length(endPointAddr);
	
	if (iLen > EndPointLen) {
		DebugAssertMsg(false, "올바르지 않은 EndPoint 형식입니다. 문자열 길이가 최대로 가능한 EndPoint 길이를 초과합니다.");
		return Invalid;
	}

	IPv4EndPointString ep{};
	ep.CopyFrom(endPointAddr);
	const int idx = ep.Find(":");

	if (idx == -1) {
		DebugAssertMsg(false, "올바르지 않은 EndPoint 형식입니다. 구분자 ':'를 찾지 못했습니다.");
		return Invalid;
	}


	IPv4EndPointString ipAddrString{};
	IPv4EndPointString portString{};

	ipAddrString.CopyFrom(0, idx - 1, const_cast<char*>(ep.Source));
	portString.CopyFrom(idx + 1, ep.Length() - 1, const_cast<char*>(ep.Source));

	const int iPortStrLen = portString.Length();

	int iPort = -1;
	if (!StringUtil::TryToNumber<Int>(iPort, portString.Source)) {
		DebugAssert(false, "올바른 포트번호가 아닙니다.");
		return Invalid;
	}

	if (iPort < 0 || iPort > 0xffff) {
		DebugAssert(false, "올바르지 않은 EndPoint 형식입니다. 포트번호는 0이상 65535이하만 가능합니다.");
		return Invalid;
	}

	return IPv4EndPoint(IPv4Address{ ipAddrString.Source }, static_cast<short>(iPort));
}

IPv4EndPoint IPv4EndPoint::Parse(const String& endPointAddr) {
	return Parse(endPointAddr.Source());
}

NS_JNET_END
