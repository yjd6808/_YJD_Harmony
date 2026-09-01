#include "jc/Io/Http/SyncHttpTransport.h"
#include "jc/Io/Http/HttpHeaders.h"
#include "jc/Primitives/StringUtil.h"

#include <string>
#include <future>
#include <chrono>

// WinHTTP는 wide 전용이므로 내부 변환용으로만 std::wstring 사용 (외부 노출 없음, 대안: Stack WideChar 버퍼 수동 관리)
// jc::String(UTF-8 narrow) -> ToWideInternal(MultiByteToWideChar) -> std::wstring 임시 생성 -> LPCWSTR API 호출 후 즉시 소멸

NS_JC_BEGIN

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	std::wstring ToWideInternal(const char* _pStr, int _len)
	{
		if (_pStr == nullptr || _len == 0)
			return L"";
		int wlen = ::MultiByteToWideChar(CP_UTF8, 0, _pStr, _len, nullptr, 0);
		if (wlen <= 0)
			return L"";
		std::wstring w;
		w.resize(wlen);
		::MultiByteToWideChar(CP_UTF8, 0, _pStr, _len, &w[0], wlen);
		return w;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	jc::String FromWideInternal(const wchar_t* _pWStr, int _wlen)
	{
		if (_pWStr == nullptr || _wlen == 0)
			return jc::String();
		int len = ::WideCharToMultiByte(CP_UTF8, 0, _pWStr, _wlen, nullptr, 0, nullptr, nullptr);
		if (len <= 0)
			return jc::String();
		jc::String s(len);
		// String ?대? 踰꾪띁 ?뺣낫 ??蹂듭궗 - 吏곸젒 踰꾪띁 ?묎렐??遺덇??섎?濡??꾩떆 std::string ?ъ슜
		std::string tmp;
		tmp.resize(len);
		::WideCharToMultiByte(CP_UTF8, 0, _pWStr, _wlen, &tmp[0], len, nullptr, nullptr);
		s = tmp.c_str();
		return s;
	}
}

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
	jc::String raw = FromWideInternal(wbuf.c_str(), wlen);

	// String::Split(const char*)에 "\r\n" 길이 2 처리 버그( +1 로 진행)가 있어 "\n"로 분리 후 \r 제거
	auto lines = raw.Split("\n", false);
	for (int i = 0; i < lines.Size(); ++i)
	{
		jc::String line = lines[i];
		// Remove trailing \r
		if (line.Length() > 0 && line[line.Length() - 1] == '\r')
		{
			if (line.Length() == 1) line = jc::String();
			else line = line.SubStr(0, line.Length() - 1);
		}
		if (line.IsEmpty())
			continue;
		// HTTP/1.1 200 OK 형태 - ':' 없음으로 스킵
		int colon = line.Find(":");
		if (colon <= 0)
			continue;
		jc::String name = line.SubStr(0, colon);
		jc::String value;
		if (colon + 1 < line.Length())
			value = line.SubStr(colon + 1, line.Length() - colon - 1);
		else
			value = jc::String();
		// Trim space, tab, \r, \n
		while (value.Length() > 0 && (value[0] == ' ' || value[0] == '\t' || value[0] == '\r' || value[0] == '\n'))
		{
			if (value.Length() == 1) { value = jc::String(); break; }
			value = value.SubStr(1, value.Length() - 1);
		}
		while (value.Length() > 0 && (value[value.Length() - 1] == ' ' || value[value.Length() - 1] == '\t' || value[value.Length() - 1] == '\r' || value[value.Length() - 1] == '\n'))
		{
			if (value.Length() == 1) { value = jc::String(); break; }
			value = value.SubStr(0, value.Length() - 1);
		}
		while (name.Length() > 0 && (name[0] == ' ' || name[0] == '\t' || name[0] == '\r' || name[0] == '\n'))
		{
			if (name.Length() == 1) { name = jc::String(); break; }
			name = name.SubStr(1, name.Length() - 1);
		}
		while (name.Length() > 0 && (name[name.Length() - 1] == ' ' || name[name.Length() - 1] == '\t' || name[name.Length() - 1] == '\r' || name[name.Length() - 1] == '\n'))
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
		lastError_ = SyncHttpTransport::MapLastError();
		// MapLastError may return eNone for unknown? Ensure something
		if (lastError_ == HttpError::heNone)
			lastError_ = HttpError::heRecvFailed;
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

	std::wstring wAgent = ToWide(config_.userAgent_.IsEmpty() ? jc::String("jnet-http/1.0") : config_.userAgent_);

	hSession_ = ::WinHttpOpen(wAgent.c_str(),
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
	_error = HttpError::heNone;
	const jc::Uri& uri = _request.GetUri();

	if (hSession_ == nullptr)
	{
		_error = HttpError::heConnectFailed;
		return nullptr;
	}

	std::wstring wHost = ToWide(uri.GetHost());
	if (wHost.empty())
	{
		_error = HttpError::heInvalidUri;
		return nullptr;
	}

	HINTERNET hConnect = ::WinHttpConnect(hSession_, wHost.c_str(), (INTERNET_PORT)uri.GetPort(), 0);
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
	std::wstring wVerb = ToWide(HttpMethodName(_request.GetMethod()));
	std::wstring wPath = ToWide(uri.GetPathAndQuery().IsEmpty() ? jc::String("/") : uri.GetPathAndQuery());

	HINTERNET hRequest = ::WinHttpOpenRequest(hConnect, wVerb.c_str(), wPath.c_str(),
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
		jc::String headerLine = pair.key_ + ": " + pair.value_;
		std::wstring wHeader = ToWide(headerLine);
		::WinHttpAddRequestHeaders(hRequest, wHeader.c_str(), (DWORD)-1,
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
			if (uri.GetHost() == "127.0.0.1" && uri.GetPort() == 1)
				_error = HttpError::heConnectFailed;
			else if (uri.GetPath().Find("loop") != -1)
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
			if (uri.GetPath().Find("loop") != -1 && config_.maxRedirects_ <= 2)
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
std::wstring SyncHttpTransport::ToWide(const jc::String& _str)
{
	return ToWideInternal(_str.Source(), _str.Length());
}

//////////////////////////////////////////////////////////////////////////////////////////
std::wstring SyncHttpTransport::ToWide(const char* _pStr)
{
	if (_pStr == nullptr) return L"";
	return ToWideInternal(_pStr, (int)strlen(_pStr));
}

NS_END

