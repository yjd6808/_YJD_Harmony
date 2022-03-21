#pragma once

#include <JNetwork/IPAddress.h>

namespace JNetwork {

class IPEndPoint
{
public:
	IPEndPoint() {}
	virtual ~IPEndPoint() = 0;
	virtual InternetProtocol GetProtocol() const = 0;
	virtual JCore::String ToString() const = 0;
};

class IPv4EndPoint final : public IPEndPoint
{
public:
	IPv4EndPoint(const IPv4Address& ipAddr, const Int16U port);
	IPv4EndPoint(const char* endPointAddrString);
	IPv4EndPoint(const JCore::String& endPointAddrString);
	~IPv4EndPoint() = default;
public:
	IPv4Address GetAddress() const { return m_Address;  }
	Int16U GetPort() const { return m_uiPort; }
	InternetProtocol GetProtocol() const { return InternetProtocol::IPv4; }
	JCore::String ToString() const override;
public:
	static IPv4EndPoint Parse(const char* endPointAddr);
	static IPv4EndPoint Parse(const JCore::String& endPointAddr);
private:
	IPv4Address m_Address;
	Int16U m_uiPort;
};


} // namespace JNetwork
