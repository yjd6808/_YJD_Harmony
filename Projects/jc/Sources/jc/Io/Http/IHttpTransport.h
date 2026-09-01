#pragma once

#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"

#include "jc/Io/Http/HttpTypes.h"
#include "jc/Io/Http/HttpHeaders.h"

NS_JC_BEGIN

class HttpRequest;

class JC_NOVTABLE IHttpConnection
{
public:
	virtual ~IHttpConnection() = default;

	virtual int GetStatusCode() const = 0;
	virtual bool ReadHeaders(OUT HttpHeaders& _headers) = 0;
	virtual int ReadBody(void* _pBuffer, int _len) = 0;
	virtual HttpError GetLastError() const = 0;
	virtual void Close() = 0;
};

using IHttpConnectionPtr = jc::SharedPtr<IHttpConnection>;

class JC_NOVTABLE IHttpTransport
{
public:
	virtual ~IHttpTransport() = default;

	virtual bool Initialize(const HttpServiceConfig& _config) = 0;
	virtual void Shutdown() = 0;
	virtual IHttpConnectionPtr Open(const HttpRequest& _request, OUT HttpError& _error) = 0;
};

using IHttpTransportPtr = jc::SharedPtr<IHttpTransport>;

NS_END

