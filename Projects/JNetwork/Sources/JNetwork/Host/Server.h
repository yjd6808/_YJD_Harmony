#pragma once

#include <JNetwork/Host/Host.h>

NS_JNET_BEGIN

class JCORE_NOVTABLE Server : public Host
{
public:
	Server(const IOCPPtr& iocp) : Host(iocp) {}
	~Server() override = default;

	Type GetType() const override { return eListener; }
	virtual bool Start(const IPv4EndPoint& localEndPoint) = 0;
	virtual bool Stop() = 0;
};

NS_JNET_END