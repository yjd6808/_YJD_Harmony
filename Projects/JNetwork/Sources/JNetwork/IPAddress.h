/*
	작성자 : 윤정도

	IP주소를 기본적으로 Host Byte Order로 저장한다.
	IPv4 주소, IPv6 주소를 다루는 클래스
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/String.h>

#include <JNetwork/Protocol.h>

namespace JNetwork {

#ifdef UNICODE
	constexpr Int32U IPv4Len_v = 40;
	constexpr Int32U IPv6Len_v = 80;
#else
	constexpr Int32U IPv4Len_v = 20;
	constexpr Int32U IPv6Len_v = 40;
#endif

class IPAddress 
{
public:
	IPAddress() = default;
	virtual ~IPAddress() = default;
	virtual InternetProtocol GetProtocol() const = 0;
	virtual JCore::String ToString() const = 0;
};

class IPv4Address : public IPAddress
{
public:
	IPv4Address() = default;
	IPv4Address(Int32UL hostOrderedAddress) { m_Addr.Addr = hostOrderedAddress; }
	IPv4Address(const char* hostOrderedAddressString);
	~IPv4Address() override = default;
public:
	JCore::String ToString() const override;
	InternetProtocol GetProtocol() const override { return InternetProtocol::IPv4; }
	Int32UL GetAddress() const { return m_Addr.Addr; }

	/// <summary>
	/// 127.0.0.1
	/// 
	/// ex) GetAddressOctet(0) -> 127
	///     GetAddressOctet(1) -> 0
	///     GetAddressOctet(2) -> 0
	///     GetAddressOctet(3) -> 1
	///     GetAddressOctet(4) -> 예외 던짐
	/// </summary>
	Byte GetAddressOctet(int idx) const;

	bool operator==(const IPv4Address& other) const { return m_Addr.Addr == other.m_Addr.Addr; }
	bool operator==(const char* other) const { return m_Addr.Addr == Parse(other).m_Addr.Addr; }
	bool operator==(const JCore::String& other) const { return m_Addr.Addr == Parse(other).m_Addr.Addr; }
	bool operator!=(const IPv4Address& other) const { return !this->operator==(other); }
	bool operator!=(const char* other) const { return !this->operator==(other); }
	bool operator!=(const JCore::String& other) const { return !this->operator==(other); }
public:
	static IPv4Address Any() {
		return { INADDR_ANY };
	}
	static IPv4Address Parse(const char* hostOrderedAddressString);
	static IPv4Address Parse(const JCore::String& hostOrderedAddressString) { return Parse(hostOrderedAddressString.Source()); }

private:
	union
	{
		Byte Seg[4];
		Int32UL Addr{};
	} m_Addr{};
};


class IPv6Address
{
	// UNUSED
public:
	static InternetProtocol GetProtocol() { return InternetProtocol::IPv6; }
private:
	Int64U m_uiAddrHigh = 0;
	Int64U m_uiAddrLow = 0;
};

} // namespace JNetwork