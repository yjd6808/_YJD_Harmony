#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/IO/Engine/IIOSource.h"
#include "jc/IO/Http/HttpRequest.h"
#include "jc/IO/Http/HttpResponse.h"
#include "jc/IO/Http/IHttpTransport.h"

NS_JC_BEGIN

// HTTP 원천 — IHttpTransport 위에서 동작. 상태/헤더는 호출자의 _spResponse에 직접 기록.
//   _failNon2xx: 파일 다운로드는 2xx만 성공으로 간주, 메모리 전송은 상태 코드를 결과로 전달
class HttpSource final : public IIOSource
{
public:
	HttpSource(const HttpRequest& _request, IHttpTransport& _transport, bool _failNon2xx, const HttpResponsePtr& _spResponse);
	~HttpSource() override;

	bool Open(OUT IOSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IOError GetLastError() const override { return lastError_; }
	_s32 GetChannelError() const override { return channelError_; }
	IOType GetType() const override { return IOType::Http; }

private:
	HttpRequest request_;
	IHttpTransport& transport_;
	bool failNon2xx_;
	HttpResponsePtr spResponse_;	// 호출자 소유 — 상태/헤더 기록처 (nullptr 허용)

	IHttpConnectionPtr spConn_;
	IOError lastError_ = ieNone;
	_s32 channelError_ = 0;			// WinHTTP 에러 코드 — R4 무손실
};

NS_END
