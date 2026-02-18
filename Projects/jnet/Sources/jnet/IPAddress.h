/*
	작성자 : 윤정도

	IP주소를 기본적으로 Host Byte Order로 저장한다.
	IPv4 주소, IPv6 주소를 다루는 클래스
 */

#pragma once

#include <jc/Type.h>
#include <jc/Primitives/String.h>

#include <jnet/Protocol.h>


NS_JNET_BEGIN
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
	virtual jc::String ToString() const = 0;
};

class IPv4Address : public IPAddress
{
public:
	IPv4Address() = default;

	explicit IPv4Address(Int32UL _hostOrderedAddress)
	: address_{.addr_ = _hostOrderedAddress}
	{
	}

	explicit IPv4Address(const char* _pHostOrderedAddressString);
	~IPv4Address() override = default;

public:
	jc::String ToString() const override;

	InternetProtocol GetProtocol() const override
	{
		return InternetProtocol::IPv4;
	}

	Int32UL GetAddress() const
	{
		return address_.addr_;
	}

	/// <summary>
	/// 127.0.0.1
	///
	/// ex) GetAddressOctet(0) -> 127
	///     GetAddressOctet(1) -> 0
	///     GetAddressOctet(2) -> 0
	///     GetAddressOctet(3) -> 1
	///     GetAddressOctet(4) -> 예외 던짐
	/// </summary>
	Byte GetAddressOctet(int _index) const;

	bool operator==(const IPv4Address& _other) const
	{
		return address_.addr_ == _other.address_.addr_;
	}

	bool operator==(const char* _pOther) const
	{
		return address_.addr_ == Parse(_pOther).address_.addr_;
	}

	bool operator==(const jc::String& _other) const
	{
		return address_.addr_ == Parse(_other).address_.addr_;
	}

	bool operator!=(const IPv4Address& _other) const
	{
		return !(*this == _other);
	}

	bool operator!=(const char* _pOther) const
	{
		return !(*this == _pOther);
	}

	bool operator!=(const jc::String& _other) const
	{
		return !(*this == _other);
	}

public:
	static IPv4Address Any()
	{
		return IPv4Address{INADDR_ANY};
	}

	static IPv4Address Parse(const char* _pHostOrderedAddressString);

	static IPv4Address Parse(const jc::String& _hostOrderedAddressString)
	{
		return Parse(_hostOrderedAddressString.Source());
	}

private:
	union
	{
		Byte seg_[4];
		Int32UL addr_{};
	} address_{};
};


class IPv6Address
{
	// UNUSED
public:
	static InternetProtocol GetProtocol()
	{
		return InternetProtocol::IPv6;
	}

private:
	Int64U addrHigh_ = 0;
	Int64U addrLow_ = 0;
};

NS_END
