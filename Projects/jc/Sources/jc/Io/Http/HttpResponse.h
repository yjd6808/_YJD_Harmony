#pragma once

#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"

#include "jc/Io/Http/HttpTypes.h"
#include "jc/Io/Http/HttpHeaders.h"
#include "jc/Io/Http/IHttpTransport.h"

NS_JC_BEGIN

class HttpResponse
{
public:
	HttpResponse() = default;
	~HttpResponse();

	HttpResponse(const HttpResponse&) = delete;
	HttpResponse& operator=(const HttpResponse&) = delete;
	HttpResponse(HttpResponse&&) = delete;
	HttpResponse& operator=(HttpResponse&&) = delete;

	int GetStatusCode() const { return statusCode_; }
	bool IsSuccess() const { return statusCode_ >= 200 && statusCode_ < 300; }
	const HttpHeaders& GetHeaders() const { return headers_; }
	HttpHeaders& GetHeaders() { return headers_; }

	jc::MemoryStream* GetBody() const { return spBody_.Get(); }
	jc::MemoryStreamPtr TakeBody();
	bool IsTaken() const { return taken_; }

	void SetStatusCode(int _code) { statusCode_ = _code; }
	void AttachBody(jc::MemoryStreamPtr _spBody) { spBody_ = _spBody; }
	void AttachConnection(IHttpConnectionPtr _spConn) { spConn_ = _spConn; }

	int ReadBody(void* _pBuffer, int _len);
	void CloseBody();

private:
	friend class HttpClient;
	friend class WinHttpTransport;
	friend class HttpService;

	int statusCode_ = 0;
	HttpHeaders headers_;
	jc::MemoryStreamPtr spBody_;
	IHttpConnectionPtr spConn_;
	bool taken_ = false;
};

using HttpResponsePtr = jc::SharedPtr<HttpResponse>;

NS_END

