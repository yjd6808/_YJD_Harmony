#include "jc/IO/Engine/HttpSource.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
HttpSource::HttpSource(const HttpRequest& _request, IHttpTransport& _transport, bool _failNon2xx)
	: request_(_request)
	, transport_(_transport)
	, failNon2xx_(_failNon2xx)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpSource::~HttpSource()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpSource::Open(OUT IOSourceInfo& _info)
{
	HttpError err = heNone;
	spConn_ = transport_.Open(request_, OUT err);
	if (spConn_ == nullptr)
	{
		lastError_ = err == heInvalidUri ? ieInvalidUri : ieOpenFailed;
		detail_.channelError_ = (int)err;
		detail_.httpStatus_ = 0;
		return false;
	}

	spHeaders_ = MakeShared<HttpHeaders>();
	if (!spConn_->ReadHeaders(OUT *spHeaders_))
	{
		lastError_ = ieReadFailed;
		detail_.channelError_ = (int)spConn_->GetLastError();
		spConn_->Close();
		spConn_ = nullptr;
		return false;
	}

	const int status = spConn_->GetStatusCode();
	_info.httpStatus_ = status;
	_info.spHeaders_ = spHeaders_;
	_info.totalBytes_ = spHeaders_->GetContentLength();

	if (failNon2xx_ && !(status >= 200 && status < 300))
	{
		lastError_ = ieHttpStatusFailed;
		detail_.channelError_ = (int)heHttpStatusFailed;
		detail_.httpStatus_ = status;
		spConn_->Close();
		spConn_ = nullptr;
		return false;
	}

	lastError_ = ieNone;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 HttpSource::Read(_byte* _pBuffer, _s32 _len)
{
	if (spConn_ == nullptr)
	{
		lastError_ = ieReadFailed;
		return -1;
	}

	const int n = spConn_->ReadBody(_pBuffer, _len);
	if (n < 0)
	{
		lastError_ = ieReadFailed;	// 전송 단계 상세는 channelError_ 로 보존 (TLS/타임아웃 등)
		detail_.channelError_ = (int)spConn_->GetLastError();
	}
	return n;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HttpSource::Close()
{
	if (spConn_ != nullptr)
	{
		spConn_->Close();
		spConn_ = nullptr;
	}
}

NS_END
