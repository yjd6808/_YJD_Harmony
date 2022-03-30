#pragma once

#include <JNetwork/IPAddress.h>

namespace JNetwork {

class IPEndPoint
{
public:
	IPEndPoint() = default;
	virtual ~IPEndPoint() = 0;
	virtual InternetProtocol GetProtocol() const = 0;
	virtual JCore::String ToString() const = 0;
};

class IPv4EndPoint final : public IPEndPoint
{
public:
	IPv4EndPoint();
	IPv4EndPoint(const IPv4Address& ipAddr, Int16U port);
	IPv4EndPoint(const char* endPointAddrString);
	IPv4EndPoint(const JCore::String& endPointAddrString);
	~IPv4EndPoint() override = default;
public:
	IPv4Address GetAddress() const { return m_Address;  }
	Int16U GetPort() const { return m_uiPort; }
	InternetProtocol GetProtocol() const override { return InternetProtocol::IPv4; }
	JCore::String ToString() const override;

	bool operator==(const IPv4EndPoint& other) const { return m_Address == other.m_Address && m_uiPort == other.m_uiPort; }
	bool operator==(const char* other) const { return *this == Parse(other); }
	bool operator==(const JCore::String& other) const { return *this == Parse(other); }
	bool operator!=(const IPv4EndPoint& other) const { return !this->operator==(other); }
	bool operator!=(const char* other) const { return !this->operator==(other); }
	bool operator!=(const JCore::String& other) const { return !this->operator==(other); }
public:
	static IPv4EndPoint Parse(const char* endPointAddr);
	static IPv4EndPoint Parse(const JCore::String& endPointAddr);
private:
	IPv4Address m_Address;
	Int16U m_uiPort;
};


class IPv6EndPoint final : public IPEndPoint
{
	// UNUSED

public:
	InternetProtocol GetProtocol() const override { return InternetProtocol::IPv6; }
	JCore::String ToString() const override { return ""; }
	
};

} // namespace JNetwork

