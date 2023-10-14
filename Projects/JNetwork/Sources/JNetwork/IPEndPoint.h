/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Natvis/NatvisByteOrder.h>
#include <JNetwork/IPAddress.h>

NS_JNET_BEGIN

class IPEndPoint
{
public:
	IPEndPoint() = default;
	virtual ~IPEndPoint() = default;
	virtual InternetProtocol GetProtocol() const = 0;
	virtual JCore::String ToString() const = 0;
};


class IPv4EndPoint final : public IPEndPoint
{
public:
	IPv4EndPoint();
	IPv4EndPoint(const IPv4EndPoint& other);
	IPv4EndPoint(const SOCKADDR_IN& other);
	IPv4EndPoint(const char* endPointString);
	IPv4EndPoint(const JCore::String& endPointString);
	IPv4EndPoint(IPv4Address addr, Int16U port);

	InternetProtocol GetProtocol() const override;
	JCore::String ToString() const override;

	IPv4Address GetAddress() const;
	Int16U GetPort() const;
	int Size() const { return sizeof(InternetAddr); }

	bool IsValidRemoteEndPoint() const;
	bool IsValidEndPoint() const;

	IPv4EndPoint& operator=(const IPv4EndPoint& other);
	bool operator==(const IPv4EndPoint& other) const;
	bool operator==(const char* other) const { return *this == Parse(other); }
	bool operator==(const JCore::String& other) const { return *this == Parse(other); }
	bool operator!=(const IPv4EndPoint& other) const { return !this->operator==(other); }
	bool operator!=(const char* other) const { return !this->operator==(other); }
	bool operator!=(const JCore::String& other) const { return !this->operator==(other); }

	static IPv4EndPoint Parse(const char* endPointAddr);
	static IPv4EndPoint Parse(const JCore::String& endPointAddr);
	
	SOCKADDR_IN InternetAddr;

	static IPv4EndPoint Invalid;
};


class IPv6EndPoint final : public IPEndPoint
{
	// UNUSED

public:
	InternetProtocol GetProtocol() const override { return InternetProtocol::IPv6; }
	JCore::String ToString() const override { return ""; }
	
};

NS_JNET_END

