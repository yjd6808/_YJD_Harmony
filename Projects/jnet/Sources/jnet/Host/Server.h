#pragma once

#include "jnet/Host/Host.h"

NS_JNET_BEGIN

class JC_NOVTABLE Server : public Host
{
public:
	Server(const IOCPPtr& _pIocp)
	: Host(_pIocp)
	{
	}

	~Server() override = default;

	Type GetType() const override
	{
		return eListener;
	}

	virtual bool Start(const IPv4EndPoint& _localEndPoint) = 0;
	virtual bool Stop() = 0;
};

NS_END
