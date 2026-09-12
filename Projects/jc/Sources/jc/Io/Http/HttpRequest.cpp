#include "jc/IO/Http/HttpRequest.h"
#include "jc/Debug/New.h"
#include "jc/Primitives/StringConvert.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest::HttpRequest(const jc::Uri& _uri)
	: uri_(_uri)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest::HttpRequest(const jc::String& _uri)
	: uri_(_uri)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest::HttpRequest(const char* _uri)
	: uri_(_uri)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetMethod(HttpMethod _method)
{
	method_ = _method;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetHeader(const jc::String& _name, const jc::String& _value)
{
	headers_.Set(_name, _value);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetBody(const jc::MemoryStreamPtr& _spBody)
{
	spBody_ = _spBody;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetBody(const jc::String& _body)
{
	if (_body.IsEmpty())
	{
		spBody_ = nullptr;
		return *this;
	}
	// 와이어는 narrow(UTF-8) 고정. _char 단위가 아닌 바이트 단위로 담는다.
	const AString utf8Body = StringConvert::ToUtf8(_body);
	spBody_ = jc::MakeShared<jc::MemoryStream>((_u32)utf8Body.Length());
	spBody_->Write(utf8Body.Source(), (_u32)utf8Body.Length());
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetConnectTimeout(int _ms)
{
	connectTimeoutMs_ = _ms;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpRequest& HttpRequest::SetReceiveTimeout(int _ms)
{
	receiveTimeoutMs_ = _ms;
	return *this;
}

NS_END

