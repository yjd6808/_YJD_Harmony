/*
	작성자 : 윤정도

	IP주소를 기본적으로 Host Byte Order로 저장한다.
	IPv4 주소, IPv6 주소를 다루는 클래스
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
	IPv4EndPoint(const IPv4EndPoint& _other);
	IPv4EndPoint(const SOCKADDR_IN& _other);
	explicit IPv4EndPoint(const char* _pEndPointString);
	explicit IPv4EndPoint(const JCore::String& _endPointString);
	IPv4EndPoint(IPv4Address _address, Int16U _port);

	InternetProtocol GetProtocol() const override;
	JCore::String ToString() const override;

	IPv4Address GetAddress() const;
	Int16U GetPort() const;
	int Size() const { return sizeof(InternetAddr); }

	bool IsValidRemoteEndPoint() const;
	bool IsValidEndPoint() const;

	IPv4EndPoint& operator=(const IPv4EndPoint& _other);
	bool operator==(const IPv4EndPoint& _other) const;
	bool operator==(const char* _pOther) const { return *this == Parse(_pOther); }
	bool operator==(const JCore::String& _other) const { return *this == Parse(_other); }
	bool operator!=(const IPv4EndPoint& _other) const { return !(*this == _other); }
	bool operator!=(const char* _pOther) const { return !(*this == _pOther); }
	bool operator!=(const JCore::String& _other) const { return !(*this == _other); }

	static IPv4EndPoint Parse(const char* _pEndPointAddr);
	static IPv4EndPoint Parse(const JCore::String& _endPointAddr);

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
