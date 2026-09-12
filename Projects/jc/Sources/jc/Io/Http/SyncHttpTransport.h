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
	mutable HttpError lastError_ = HttpError::heSendFailed;
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

private:
	HINTERNET hSession_ = nullptr;
	HttpServiceConfig config_;
};

NS_END

