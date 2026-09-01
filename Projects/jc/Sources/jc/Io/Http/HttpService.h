#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * HttpService — HTTP 도메인 창구 (구 HttpClient API 승계, R5 표현력 보존)
 *   SendAsync/GetAsync  : 메모리 목적지 — 상태 코드를 결과로 전달 (404도 전송 성공, error_ == ieNone)
 *   DownloadAsync       : 파일 목적지 — 2xx만 성공(ieHttpStatusFailed + detail_.httpStatus_ 보존)
 *
 * 사용 예:
 *   HttpRequest req("https://api.example.com/v1/login");
 *   req.SetMethod(HttpMethod::hmPost)
 *      .SetHeader("Content-Type", "application/json")
 *      .SetHeader("Authorization", token)
 *      .SetBody(jsonBody);
 *   daemon.Http().SendAsync(req, [](const IoResult& _r) { ... });
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/Io/IoDefine.h"
#include "jc/Io/Engine/IoJobEngine.h"
#include "jc/Io/Http/HttpTypes.h"
#include "jc/Io/Http/HttpResponse.h"

NS_JC_BEGIN

class PathResolver;
class HttpRequest;
class HttpResponse;
class IHttpTransport;

class HttpService final : public NonCopyableNonMovable
{
public:
	// 비동기 — 메모리 목적지
	IoHandle SendAsync(const HttpRequest& _request, const IoCallback& _callback = nullptr);
	IoHandle GetAsync(const String& _url, const IoCallback& _callback = nullptr);

	// 비동기 — 파일 목적지 (_destPath는 resolver 별칭 경로 허용: "save:/patch/ui.pak")
	IoHandle DownloadAsync(const String& _url, const String& _destPath, const IoCallback& _callback = nullptr);

	// 동기 — _waitForBody=true: 바디 수신까지 완료 후 반환(구 hcContentReceived)
	//         false: 헤더만 받고 연결을 보관 → ReadBody로 스트리밍(구 hcHeadersReceived)
	HttpResponsePtr Send(const HttpRequest& _request, bool _waitForBody = true);
	HttpResponsePtr Get(const String& _url);

	// 동기 — 파일 다운로드
	IoResultPtr Download(const String& _url, const String& _destPath);

private:
	friend class IoDaemon;
	HttpService(PathResolver& _resolver, IoJobEngine& _engine);
	bool Initialize(const HttpServiceConfig& _config, _s64 _memoryLimit, _s32 _readUnit);
	void Shutdown();

	PathResolver& resolver_;
	IoJobEngine& engine_;
	IHttpTransport* pTransport_ = nullptr;		// Initialize에서 생성, Shutdown에서 파괴 (IOCP 교체 지점)
	HttpServiceConfig config_;
	_s64 memoryLimit_ = 256LL * 1024 * 1024;
	_s32 readUnit_ = 256 * 1024;
};

NS_END
