#include "jc/IO/Http/HttpSource.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
HttpSource::HttpSource(const HttpRequest& _request, IHttpTransport& _transport, bool _failNon2xx, const HttpResponsePtr& _spResponse)
	: request_(_request)
	, transport_(_transport)
	, failNon2xx_(_failNon2xx)
	, spResponse_(_spResponse)
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
	HttpError err = heSendFailed;
	spConn_ = transport_.Open(request_, OUT err);
	if (spConn_ == nullptr)
	{
		lastError_ = err == heInvalidUri ? ieInvalidUri : ieOpenFailed;
		channelError_ = (int)err;
		return false;
	}

	HttpHeaders headers;
	if (!spConn_->ReadHeaders(OUT headers))
	{
		lastError_ = ieReadFailed;
		channelError_ = (int)spConn_->GetLastError();
		spConn_->Close();
		spConn_ = nullptr;
		return false;
	}

	const int status = spConn_->GetStatusCode();
	_info.totalBytes_ = headers.GetContentLength();

	// 상태/헤더는 호출자의 응답 그릇에 직접 기록 — 실패 시에도 부분 수신 메타 보존 (P0-3)
	if (spResponse_ != nullptr)
	{
		spResponse_->SetStatusCode(status);
		spResponse_->GetHeaders() = headers;
	}

	if (failNon2xx_ && !(status >= 200 && status < 300))
	{
		lastError_ = ieHttpStatusFailed;
		channelError_ = (int)heHttpStatusFailed;
		spConn_->Close();
		spConn_ = nullptr;
		return false;
	}

	lastError_ = ieNone;
	channelError_ = 0;
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
		channelError_ = (int)spConn_->GetLastError();
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
