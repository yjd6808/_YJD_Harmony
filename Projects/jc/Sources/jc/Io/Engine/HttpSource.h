#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Io/Engine/IIoSource.h"
#include "jc/Io/Http/HttpRequest.h"
#include "jc/Io/Http/IHttpTransport.h"

NS_JC_BEGIN

// HTTP 원천 (구 HttpDataSource 정식 승격) — IHttpTransport 위에서 동작
//   _failNon2xx : 파일 다운로드는 2xx만 성공으로 간주(구 규약 보존), 메모리 전송은 상태 코드를 결과로 전달
class HttpSource final : public IIoSource
{
public:
	HttpSource(const HttpRequest& _request, IHttpTransport& _transport, bool _failNon2xx);
	~HttpSource() override;

	bool Open(OUT IoSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IoError GetLastError() const override { return lastError_; }
	IoErrorDetail GetLastErrorDetail() const override { return detail_; }

private:
	HttpRequest request_;
	IHttpTransport& transport_;
	bool failNon2xx_;

	IHttpConnectionPtr spConn_;
	SharedPtr<HttpHeaders> spHeaders_;	// 응답 헤더 — IoResult.http_.headers_ 로 전달
	IoError lastError_ = ieNone;
	IoErrorDetail detail_;
};

NS_END
