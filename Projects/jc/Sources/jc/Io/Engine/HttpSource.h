#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/IO/Engine/IIOSource.h"
#include "jc/IO/Http/HttpRequest.h"
#include "jc/IO/Http/IHttpTransport.h"

NS_JC_BEGIN

class HttpSource final : public IIOSource
{
public:
	HttpSource(const HttpRequest& _request, IHttpTransport& _transport, bool _failNon2xx);
	~HttpSource() override;

	bool Open(OUT IOSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IOError GetLastError() const override { return lastError_; }
	IOErrorDetail GetLastErrorDetail() const override { return detail_; }

private:
	HttpRequest request_;
	IHttpTransport& transport_;
	bool failNon2xx_;

	IHttpConnectionPtr spConn_;
	SharedPtr<HttpHeaders> spHeaders_;	// 응답 헤더 — IOResult.http_.headers_ 로 전달
	IOError lastError_ = ieNone;
	IOErrorDetail detail_;
};

NS_END
