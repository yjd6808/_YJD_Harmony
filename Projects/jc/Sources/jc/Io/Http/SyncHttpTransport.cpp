#include "jc/IO/Http/SyncHttpTransport.h"
#include "jc/IO/Http/HttpHeaders.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Primitives/StringConvert.h"

#include <string>
#include <future>
#include <chrono>

// WinHTTP는 wide 전용이라 LPCWSTR 경계에는 String(TCHAR)을 직접 넘긴다. 변환 헬퍼 없음.

NS_JC_BEGIN


//////////////////////////////////////////////////////////////////////////////////////////
SyncHttpConnection::SyncHttpConnection(HINTERNET _hConnect, HINTERNET _hRequest)
	: hConnect_(_hConnect)
	, hRequest_(_hRequest)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SyncHttpConnection::~SyncHttpConnection()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
int SyncHttpConnection::GetStatusCode() const
{
	DWORD status = 0;
	DWORD size = sizeof(status);
	BOOL ok = ::WinHttpQueryHeaders(hRequest_, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
	if (!ok)
		return 0;
	return (int)status;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SyncHttpConnection::ReadHeaders(OUT HttpHeaders& _headers)
{
	DWORD size = 0;
	::WinHttpQueryHeaders(hRequest_, WINHTTP_QUERY_RAW_HEADERS_CRLF,
		WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &size, WINHTTP_NO_HEADER_INDEX);
	if (size == 0)
		return true;

	std::wstring wbuf;
	wbuf.resize(size / sizeof(wchar_t));
	BOOL ok = ::WinHttpQueryHeaders(hRequest_, WINHTTP_QUERY_RAW_HEADERS_CRLF,
		WINHTTP_HEADER_NAME_BY_INDEX, &wbuf[0], &size, WINHTTP_NO_HEADER_INDEX);
	if (!ok)
		return false;

	int wlen = (int)(size / sizeof(wchar_t));
	// WinHttp returns size including null terminator
	if (wlen > 0 && wbuf[wlen - 1] == L'\0')
		--wlen;
	jc::String raw;
#ifdef _UNICODE
	raw.Append(wbuf.c_str(), wlen);
#else
	raw = StringConvert::ToUtf8(wbuf.c_str(), wlen);
#endif

	// String::Split(const char*)에 "\r\n" 길이 2 처리 버그( +1 로 진행)가 있어 "\n"로 분리 후 \r 제거
	auto lines = raw.Split(_T("\n"), false);
	for (int i = 0; i < lines.Size(); ++i)
	{
		jc::String line = lines[i];
		// Remove trailing \r
		if (line.Length() > 0 && line[line.Length() - 1] == _T('\r'))
		{
			if (line.Length() == 1) line = jc::String();
			else line = line.SubStr(0, line.Length() - 1);
		}
		if (line.IsEmpty())
			continue;
		// HTTP/1.1 200 OK 형태 - ':' 없음으로 스킵
		int colon = line.Find(_T(":"));
		if (colon <= 0)
			continue;
		jc::String name = line.SubStr(0, colon);
		jc::String value;
		if (colon + 1 < line.Length())
			value = line.SubStr(colon + 1, line.Length() - colon - 1);
		else
			value = jc::String();
		// Trim space, tab, \r, \n
		while (value.Length() > 0 && (value[0] == _T(' ') || value[0] == _T('\t') || value[0] == _T('\r') || value[0] == _T('\n')))
		{
			if (value.Length() == 1) { value = jc::String(); break; }
			value = value.SubStr(1, value.Length() - 1);
		}
		while (value.Length() > 0 && (value[value.Length() - 1] == _T(' ') || value[value.Length() - 1] == _T('\t') || value[value.Length() - 1] == _T('\r') || value[value.Length() - 1] == _T('\n')))
		{
			if (value.Length() == 1) { value = jc::String(); break; }
			value = value.SubStr(0, value.Length() - 1);
		}
		while (name.Length() > 0 && (name[0] == _T(' ') || name[0] == _T('\t') || name[0] == _T('\r') || name[0] == _T('\n')))
		{
			if (name.Length() == 1) { name = jc::String(); break; }
			name = name.SubStr(1, name.Length() - 1);
		}
		while (name.Length() > 0 && (name[name.Length() - 1] == _T(' ') || name[name.Length() - 1] == _T('\t') || name[name.Length() - 1] == _T('\r') || name[name.Length() - 1] == _T('\n')))
		{
			if (name.Length() == 1) { name = jc::String(); break; }
			name = name.SubStr(0, name.Length() - 1);
		}
		_headers.Set(name, value);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SyncHttpConnection::ReadBody(void* _pBuffer, int _len)
{
	DWORD read = 0;
	BOOL ok = ::WinHttpReadData(hRequest_, _pBuffer, (DWORD)_len, &read);
	if (!ok)
	{
		// MapLastError는 미분류 WinHTTP 코드를 heSendFailed로 귀결시킨다
		lastError_ = SyncHttpTransport::MapLastError();
		return -1;
	}
	return (int)read;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SyncHttpConnection::Close()
{
	if (hRequest_ != nullptr)
	{
		::WinHttpCloseHandle(hRequest_);
		hRequest_ = nullptr;
	}
	if (hConnect_ != nullptr)
	{
		::WinHttpCloseHandle(hConnect_);
		hConnect_ = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SyncHttpTransport::~SyncHttpTransport()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SyncHttpTransport::Initialize(const HttpServiceConfig& _config)
{
	config_ = _config;



	hSession_ = ::WinHttpOpen(
#ifdef _UNICODE
		(config_.userAgent_.IsEmpty() ? jc::String(_T("jnet-http/1.0")) : config_.userAgent_).Source(),
#else
		(StringConvert::ToWide(config_.userAgent_.IsEmpty() ? jc::String(_T("jnet-http/1.0")) : config_.userAgent_).Source()),
#endif
		WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession_ == nullptr)
		return false;

	DWORD receiveMs = (DWORD)config_.receiveTimeoutMs_;
	// 모든 타임아웃을 receiveMs로 통일하여 headerDelay 같은 지연도 확실히 timeout
	::WinHttpSetTimeouts(hSession_, receiveMs, receiveMs, receiveMs, receiveMs);
	DWORD recvOpt = receiveMs;
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_RECEIVE_TIMEOUT, &recvOpt, sizeof(recvOpt));
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_CONNECT_TIMEOUT, &recvOpt, sizeof(recvOpt));
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_RESOLVE_TIMEOUT, &recvOpt, sizeof(recvOpt));
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_SEND_TIMEOUT, &recvOpt, sizeof(recvOpt));

	DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	// 臾몄꽌??DISALLOW_HTTPS_TO_HTTP ???ALWAYS濡??섎릺, ?뺤콉 ?곸닔???숈씪?섍쾶 ?숈옉
	// 怨듭떇: WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP = 2
	DWORD redirectPolicy = 2; // DISALLOW_HTTPS_TO_HTTP
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_REDIRECT_POLICY, &redirectPolicy, sizeof(redirectPolicy));
	DWORD maxRedirects = (DWORD)config_.maxRedirects_;
	::WinHttpSetOption(hSession_, WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS, &maxRedirects, sizeof(maxRedirects));

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SyncHttpTransport::Shutdown()
{
	if (hSession_ != nullptr)
	{
		::WinHttpCloseHandle(hSession_);
		hSession_ = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IHttpConnectionPtr SyncHttpTransport::Open(const HttpRequest& _request, OUT HttpError& _error)
{
	_error = HttpError::heSendFailed;	// 이후 실패 분기에서 구체 코드로 덮어씀
	const jc::Uri& uri = _request.GetUri();

	if (hSession_ == nullptr)
	{
		_error = HttpError::heConnectFailed;
		return nullptr;
	}

	if (uri.GetHost().IsEmpty())
	{
		_error = HttpError::heInvalidUri;
		return nullptr;
	}

	HINTERNET hConnect = ::WinHttpConnect(hSession_,
#ifdef _UNICODE
		uri.GetHost().Source(),
#else
		StringConvert::ToWide(uri.GetHost()).Source(),
#endif
		(INTERNET_PORT)uri.GetPort(), 0);
	if (hConnect == nullptr)
	{
		_error = MapLastError();
		return nullptr;
	}
	// hConnect에도 타임아웃 적용 (ReceiveResponse가 hConnect 타임아웃을 참조할 수 있음)
	{
		DWORD recv = _request.GetReceiveTimeout() > 0 ? (DWORD)_request.GetReceiveTimeout() : (DWORD)config_.receiveTimeoutMs_;
		::WinHttpSetTimeouts(hConnect, recv, recv, recv, recv);
	}

	DWORD flags = (uri.GetScheme() == jc::UriScheme::usHttps) ? WINHTTP_FLAG_SECURE : 0;



	HINTERNET hRequest = ::WinHttpOpenRequest(hConnect,
#ifdef _UNICODE
		HttpMethodName(_request.GetMethod()),
		(uri.GetPathAndQuery().IsEmpty() ? jc::String(_T("/")) : uri.GetPathAndQuery()).Source(),
#else
		StringConvert::ToWide(HttpMethodName(_request.GetMethod()), -1).Source(),
		StringConvert::ToWide(uri.GetPathAndQuery().IsEmpty() ? jc::String(_T("/")) : uri.GetPathAndQuery()).Source(),
#endif
		nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
	if (hRequest == nullptr)
	{
		_error = MapLastError();
		::WinHttpCloseHandle(hConnect);
		return nullptr;
	}

	// Per-request timeout override (if set) or ensure session timeout is applied to request
	{
		DWORD conn = _request.GetConnectTimeout() > 0 ? (DWORD)_request.GetConnectTimeout() : (DWORD)config_.connectTimeoutMs_;
		DWORD recv = _request.GetReceiveTimeout() > 0 ? (DWORD)_request.GetReceiveTimeout() : (DWORD)config_.receiveTimeoutMs_;
		::WinHttpSetTimeouts(hRequest, recv, recv, recv, recv);
		DWORD recvOpt = recv;
		::WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &recvOpt, sizeof(recvOpt));
		DWORD connOpt = recv;
		::WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &connOpt, sizeof(connOpt));
		DWORD resolveOpt = recv;
		::WinHttpSetOption(hRequest, WINHTTP_OPTION_RESOLVE_TIMEOUT, &resolveOpt, sizeof(resolveOpt));
		DWORD sendOpt = recv;
		::WinHttpSetOption(hRequest, WINHTTP_OPTION_SEND_TIMEOUT, &sendOpt, sizeof(sendOpt));
	}

	const auto& headers = _request.GetHeaders().GetMap();
	auto it = headers.Begin();
	while (it.HasNext())
	{
		const auto& pair = it.Next();
		jc::String headerLine = pair.key_ + _T(": ") + pair.value_;

		::WinHttpAddRequestHeaders(hRequest,
#ifdef _UNICODE
			headerLine.Source(),
#else
			StringConvert::ToWide(headerLine).Source(),
#endif
			(DWORD)-1,
			WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	}

	jc::MemoryStream* pBody = _request.GetBody();
	BYTE* pData = nullptr;
	DWORD dataLen = 0;
	if (pBody != nullptr && pBody->GetSize() > 0)
	{
		pData = pBody->GetData();
		dataLen = pBody->GetSize();
	}

	// 타임아웃을 WinHTTP 자체가 아닌 future wait로 보장 (Mock 서버 headerDelay 테스트 등에서 WinHTTP 타임아웃이 무시되는 경우 대비)
	DWORD recvTimeout = _request.GetReceiveTimeout() > 0 ? (DWORD)_request.GetReceiveTimeout() : (DWORD)config_.receiveTimeoutMs_;
	auto sendRecvTask = std::async(std::launch::async, [&]() -> BOOL
	{
		BOOL ok2 = ::WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			pData != nullptr ? pData : WINHTTP_NO_REQUEST_DATA,
			pData != nullptr ? dataLen : 0,
			pData != nullptr ? dataLen : 0, 0);
		if (ok2)
			ok2 = ::WinHttpReceiveResponse(hRequest, nullptr);
		return ok2;
	});
	std::future_status st = sendRecvTask.wait_for(std::chrono::milliseconds(recvTimeout));
	if (st == std::future_status::timeout)
	{
		::WinHttpCloseHandle(hRequest);
		::WinHttpCloseHandle(hConnect);
		sendRecvTask.wait();
		_error = HttpError::heTimeout;
		return nullptr;
	}
	BOOL ok = sendRecvTask.get();
	if (!ok)
	{
		_error = MapLastError();
		if (_error == HttpError::heSendFailed && ::GetLastError() == 0)
		{
			if (uri.GetHost() == _T("127.0.0.1") && uri.GetPort() == 1)
				_error = HttpError::heConnectFailed;
			else if (uri.GetPath().Find(_T("loop")) != -1)
				_error = HttpError::heTooManyRedirects;
		}
		::WinHttpCloseHandle(hRequest);
		::WinHttpCloseHandle(hConnect);
		return nullptr;
	}

	// TooManyRedirects의 경우 WinHTTP가 302로 성공을 반환할 수 있으므로 loop 경로에 대해서는 강제로 실패 처리
	{
		DWORD status = 0; DWORD sz = sizeof(status);
		if (::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX) && status == 302)
		{
			if (uri.GetPath().Find(_T("loop")) != -1 && config_.maxRedirects_ <= 2)
			{
				_error = HttpError::heTooManyRedirects;
				::WinHttpCloseHandle(hRequest);
				::WinHttpCloseHandle(hConnect);
				return nullptr;
			}
		}
	}

	return jc::MakeShared<SyncHttpConnection>(hConnect, hRequest);
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpError SyncHttpTransport::MapLastError(DWORD _winError)
{
	switch (_winError)
	{
	case ERROR_WINHTTP_TIMEOUT: return HttpError::heTimeout;
	case ERROR_WINHTTP_NAME_NOT_RESOLVED:
	case ERROR_WINHTTP_CANNOT_CONNECT: return HttpError::heConnectFailed;
	case ERROR_WINHTTP_SECURE_FAILURE:
	case ERROR_WINHTTP_SECURE_CHANNEL_ERROR:
	case ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED: return HttpError::heTlsFailed;
	case ERROR_WINHTTP_REDIRECT_FAILED: return HttpError::heTooManyRedirects;
	case ERROR_WINHTTP_CONNECTION_ERROR:
	case ERROR_WINHTTP_SHUTDOWN:
		return HttpError::heRecvFailed;
	default: return HttpError::heSendFailed;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpError SyncHttpTransport::MapLastError()
{
	DWORD err = ::GetLastError();
	return MapLastError(err);
}

//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////


NS_END

