/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/IPEndPoint.h>

#include <jc/Primitives/StaticString.h>
#include <jc/Primitives/StringUtil.h>
#include <jc/Exception.h>
#include <jc/Ascii.h>

#include <jnet/ByteOrder.h>

NS_JNET_BEGIN
USING_NS_JC;

/*=====================================================================================
							IPEndPoint
=======================================================================================*/


/*=====================================================================================
							IPv4EndPoint
=======================================================================================*/

IPv4EndPoint IPv4EndPoint::Invalid = []()
{
	SOCKADDR_IN socketAddr;
	Memory::Set(&socketAddr, sizeof(SOCKADDR_IN), 0);
	return IPv4EndPoint(socketAddr);
}();

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint()
: InternetAddr()
{
	Memory::Set(&InternetAddr, sizeof(SOCKADDR_IN), 0);
	InternetAddr.sin_family = AF_INET;
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint(const IPv4EndPoint& _other)
: InternetAddr()
{
	operator=(_other);
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint(const SOCKADDR_IN& _other)
: InternetAddr()
{
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = _other.sin_addr.s_addr;
	InternetAddr.sin_port = _other.sin_port;
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint(const char* _pEndPointString)
{
	operator=(Parse(_pEndPointString));
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint(const jc::String& _endPointString)
{
	operator=(Parse(_endPointString.Source()));
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint::IPv4EndPoint(IPv4Address _address, Int16U _port)
: InternetAddr()
{
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = ByteOrder::HostToNetwork(_address.GetAddress());
	InternetAddr.sin_port = ByteOrder::HostToNetwork(_port);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IPv4EndPoint::IsValidRemoteEndPoint() const
{
	if (InternetAddr.sin_family == AF_INET &&
		InternetAddr.sin_addr.s_addr != ADDR_ANY &&
		InternetAddr.sin_port != 0)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IPv4EndPoint::IsValidEndPoint() const
{
	if (InternetAddr.sin_family == AF_INET)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint& IPv4EndPoint::operator=(const IPv4EndPoint& _other)
{
	InternetAddr.sin_family = _other.InternetAddr.sin_family;
	InternetAddr.sin_port = _other.InternetAddr.sin_port;
	InternetAddr.sin_addr.s_addr = _other.InternetAddr.sin_addr.s_addr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IPv4EndPoint::operator==(const IPv4EndPoint& _other) const
{
	if (InternetAddr.sin_family == _other.InternetAddr.sin_family &&
		InternetAddr.sin_port == _other.InternetAddr.sin_port &&
		InternetAddr.sin_addr.s_addr == _other.InternetAddr.sin_addr.s_addr)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
InternetProtocol IPv4EndPoint::GetProtocol() const
{
	return InternetProtocol::IPv4;
}

//////////////////////////////////////////////////////////////////////////////////////////
String IPv4EndPoint::ToString() const
{
	String result = GetAddress().ToString() + ':';
	result += GetPort();
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4Address IPv4EndPoint::GetAddress() const
{
	return IPv4Address{ByteOrder::NetworkToHost(InternetAddr.sin_addr.s_addr)};
}

//////////////////////////////////////////////////////////////////////////////////////////
Int16U IPv4EndPoint::GetPort() const
{
	return ByteOrder::NetworkToHost(InternetAddr.sin_port);
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint IPv4EndPoint::Parse(const char* _pEndPointAddr)
{
	static constexpr int END_POINT_LEN = IPv4Len_v + 6; // +6 : 포트 최대 5자리 + 문자 ':'를 포함한 길이
	using IPv4EndPointString = StaticString<END_POINT_LEN>;

	const int endPointLength = StringUtil::Length(_pEndPointAddr);

	if (endPointLength > END_POINT_LEN)
	{
		jc_assert_msg(false, "올바르지 않은 EndPoint 형식입니다. 문자열 길이가 최대로 가능한 EndPoint 길이를 초과합니다.");
		return Invalid;
	}

	IPv4EndPointString endPointString{};
	endPointString.CopyFrom(_pEndPointAddr);
	const int delimiterIndex = endPointString.Find(":");

	if (delimiterIndex == -1)
	{
		jc_assert_msg(false, "올바르지 않은 EndPoint 형식입니다. 구분자 ':'를 찾지 못했습니다.");
		return Invalid;
	}

	IPv4EndPointString addressString{};
	IPv4EndPointString portString{};

	addressString.CopyFrom(0, delimiterIndex - 1, const_cast<char*>(endPointString.Source));
	portString.CopyFrom(delimiterIndex + 1, endPointString.Length() - 1, const_cast<char*>(endPointString.Source));

	const int portStringLength = portString.Length();
	(void)portStringLength; // length currently unused but kept for potential validation

	int port = -1;
	if (!StringUtil::TryToNumber<Int>(port, portString.Source))
	{
		jc_assert(false, "올바른 포트번호가 아닙니다.");
		return Invalid;
	}

	if (port < 0 || port > 0xffff)
	{
		jc_assert(false, "올바르지 않은 EndPoint 형식입니다. 포트번호는 0이상 65535이하만 가능합니다.");
		return Invalid;
	}

	return IPv4EndPoint(IPv4Address{addressString.Source}, static_cast<Int16U>(port));
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint IPv4EndPoint::Parse(const String& _endPointAddr)
{
	return Parse(_endPointAddr.Source());
}

NS_JNET_END
