#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * HttpResponse — 호출자 소유의 응답 그릇
 *   상태/헤더/바디는 HttpSource가 전송 중 직접 기록한다 (연결 보관형 스트리밍 삭제).
 *   (동기 Send는 엔진 경로로 통합 — _waitForBody 옵션은 HttpRequest로 이관 검토).
 */

#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"

#include "jc/IO/Http/HttpTypes.h"
#include "jc/IO/Http/HttpHeaders.h"

NS_JC_BEGIN

class HttpResponse
{
public:
	HttpResponse() = default;
	~HttpResponse() = default;

	HttpResponse(const HttpResponse&) = delete;
	HttpResponse& operator=(const HttpResponse&) = delete;
	HttpResponse(HttpResponse&&) = delete;
	HttpResponse& operator=(HttpResponse&&) = delete;

	int GetStatusCode() const { return statusCode_; }
	bool IsSuccess() const { return statusCode_ >= 200 && statusCode_ < 300; }
	const HttpHeaders& GetHeaders() const { return headers_; }
	HttpHeaders& GetHeaders() { return headers_; }

	jc::MemoryStream* GetBody() const { return spBody_.Get(); }
	const jc::MemoryStreamPtr& GetBodyPtr() const { return spBody_; }

	void SetStatusCode(int _code) { statusCode_ = _code; }
	void AttachBody(jc::MemoryStreamPtr _spBody) { spBody_ = _spBody; }

private:
	int statusCode_ = 0;
	HttpHeaders headers_;
	jc::MemoryStreamPtr spBody_;
};

using HttpResponsePtr = jc::SharedPtr<HttpResponse>;

NS_END
