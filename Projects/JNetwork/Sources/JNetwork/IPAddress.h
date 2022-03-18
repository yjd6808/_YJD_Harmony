/*
	�ۼ��� : ������

	IP�ּҸ� �⺻������ Host Byte Order�� �����Ѵ�.
	IPv4 �ּ�, IPv6 �ּҸ� �ٷ�� Ŭ����
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
	IPAddress() {}
	virtual ~IPAddress() = 0;
	virtual InternetProtocol GetProtocol() const = 0;
	virtual JCore::String ToString() const = 0;
};

class IPv4Address : public IPAddress
{
public:
	IPv4Address() { m_Addr.Addr = 0; }
	IPv4Address(Int32UL hostOrderedAddress) { m_Addr.Addr = hostOrderedAddress; }
	IPv4Address(const char* hostOrderedAddressString);
	virtual ~IPv4Address() = default;
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
	///     GetAddressOctet(4) -> ���� ����
	/// </summary>
	Byte GetAddressOctet(int idx) const;
public:
	static IPv4Address Any() {
		return { (Int32UL)0 };
	}
	static IPv4Address Parse(const char* addrString);
	static IPv4Address Parse(const JCore::String& addrString) { Parse(addrString.Source()); }

private:
	union
	{
		Byte Seg[4];
		Int32UL Addr = 0;
	} m_Addr;
};


class IPv6Address
{
public:
	InternetProtocol GetProtocol() const { return InternetProtocol::IPv6; }
private:
	Int64U m_uiAddrHigh = 0;
	Int64U m_uiAddrLow = 0;
};

} // namespace JNetwork