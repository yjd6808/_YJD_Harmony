#pragma once

#include "jc/IO/Http/IHttpTransport.h"
#include "jc/IO/Http/HttpRequest.h"

#include <Windows.h>
#include <winhttp.h>

NS_JC_BEGIN

class SyncHttpConnection final : public IHttpConnection
{
public:
	SyncHttpConnection(HINTERNET _hConnect, HINTERNET _hRequest);
	~SyncHttpConnection() override;

	int GetStatusCode() const override;
	bool ReadHeaders(OUT HttpHeaders& _headers) override;
	int ReadBody(void* _pBuffer, int _len) override;
	HttpError GetLastError() const override { return lastError_; }
	void Close() override;

private:
	HINTERNET hConnect_ = nullptr;
	HINTERNET hRequest_ = nullptr;
	mutable HttpError lastError_ = HttpError::heNone;
};

class SyncHttpTransport final : public IHttpTransport
{
public:
	SyncHttpTransport() = default;
	~SyncHttpTransport() override;

	bool Initialize(const HttpServiceConfig& _config) override;
	void Shutdown() override;
	IHttpConnectionPtr Open(const HttpRequest& _request, OUT HttpError& _error) override;

	static HttpError MapLastError(DWORD _winError);
	static HttpError MapLastError();

private:
	// WinHTTP는 wide 전용이므로 내부 변환용으로만 std::wstring 사용 (외부 노출 없음, 대안: Stack WideChar 버퍼 수동 관리)
	// jc::String(UTF-8) -> MultiByteToWideChar -> 스택/힙 wide 버퍼 -> LPCWSTR 로 대체 가능하나, 현재는 내부 일시 생성 후 즉시 소멸 패턴 유지
	static std::wstring ToWide(const jc::String& _str);
	static std::wstring ToWide(const char* _pStr);

private:
	HINTERNET hSession_ = nullptr;
	HttpServiceConfig config_;
};

NS_END

