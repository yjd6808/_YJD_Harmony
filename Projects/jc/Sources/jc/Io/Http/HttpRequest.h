#pragma once

#include "jc/Primitives/Uri.h"
#include "jc/Primitives/String.h"
#include "jc/Container/MemoryStream.h"

#include "jc/IO/Http/HttpTypes.h"
#include "jc/IO/Http/HttpHeaders.h"

NS_JC_BEGIN

class HttpRequest
{
public:
	HttpRequest() = default;
	explicit HttpRequest(const jc::Uri& _uri);
	explicit HttpRequest(const jc::String& _uri);
	explicit HttpRequest(const char* _uri);
	~HttpRequest() = default;

	HttpRequest(const HttpRequest& _other) = default;
	HttpRequest(HttpRequest&& _other) noexcept = default;
	HttpRequest& operator=(const HttpRequest& _other) = default;
	HttpRequest& operator=(HttpRequest&& _other) noexcept = default;

	HttpRequest& SetMethod(HttpMethod _method);
	HttpRequest& SetHeader(const jc::String& _name, const jc::String& _value);
	HttpRequest& SetBody(const jc::MemoryStreamPtr& _spBody);
	HttpRequest& SetBody(const jc::String& _body);
	HttpRequest& SetConnectTimeout(int _ms);
	HttpRequest& SetReceiveTimeout(int _ms);

	const jc::Uri& GetUri() const { return uri_; }
	HttpMethod GetMethod() const { return method_; }
	const HttpHeaders& GetHeaders() const { return headers_; }
	HttpHeaders& GetHeaders() { return headers_; }
	jc::MemoryStream* GetBody() const { return spBody_.Get(); }
	const jc::MemoryStreamPtr& GetBodyPtr() const { return spBody_; }
	int GetConnectTimeout() const { return connectTimeoutMs_; }
	int GetReceiveTimeout() const { return receiveTimeoutMs_; }

private:
	jc::Uri uri_;
	HttpMethod method_ = HttpMethod::hmGet;
	HttpHeaders headers_;
	jc::MemoryStreamPtr spBody_;
	int connectTimeoutMs_ = 0;
	int receiveTimeoutMs_ = 0;
};

NS_END

