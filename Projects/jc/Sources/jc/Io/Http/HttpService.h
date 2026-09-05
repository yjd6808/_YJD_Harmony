#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * HttpService — HTTP 도메인 창구
 *   Get/Post/Send : 메모리 목적지 — 상태 코드는 _spResponse로 판단 (404도 전송 성공, error_ == ieNone)
 *   Download      : 파일 목적지 — 2xx만 성공 (ieHttpStatusFailed)
 *   Upload        : 파일 → 원격 (P2-2)
 *   [계약] 비동기는 완료 콜백 전까지 _spBody/_spResponse 접근 금지 (잡이 배타 소유)
 *   실패 시에도 부분 수신 status/headers를 _spResponse에 보존 — nullptr 반환 경로 없음 (P0-3 해소)
 *
 * 사용 예:
 *   HttpResponsePtr spRes = MakeShared<HttpResponse>();
 *   IOResult r = daemon.Http().Get("https://api.example.com/v1/user", spRes);
 *   if (r.IsOk() && spRes->GetStatusCode() == 200)
 *   {
 *       ParseJson(spRes->GetBody());
 *   }
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/Container/MemoryStream.h"
#include "jc/IO/IOTypes.h"
#include "jc/IO/Engine/IOEngine.h"
#include "jc/IO/Http/HttpTypes.h"
#include "jc/IO/Http/HttpResponse.h"

NS_JC_BEGIN

class PathResolver;
class HttpRequest;
class IHttpTransport;

class HttpService final : public NonCopyableNonMovable
{
public:
	// 동기 — 메모리 목적지. 바디는 _spResponse의 body 스트림에 (미리 붙여둔 버퍼 재사용, 없으면 생성)
	IOResult Get(const String& _url, const HttpResponsePtr& _spResponse);
	IOResult Post(const String& _url, const MemoryStreamPtr& _spBody, const String& _contentType, const HttpResponsePtr& _spResponse);

	// 비동기 — 메모리 목적지
	//   사용 예) daemon.Http().GetAsync("https://api.example.com/v1/user", spRes,
	//                [spRes](const IOResult& _r)
	//                {
	//                    if (_r.IsOk() && spRes->GetStatusCode() == 200) ParseJson(spRes->GetBody());
	//                });
	IOHandle GetAsync(const String& _url, const HttpResponsePtr& _spResponse, const IOCallback& _callback = nullptr);
	IOHandle PostAsync(const String& _url, const MemoryStreamPtr& _spBody, const String& _contentType, const HttpResponsePtr& _spResponse, const IOCallback& _callback = nullptr);

	// 파일 목적지 — non-2xx는 실패. 상태/헤더 필요 시에만 _spResponse 전달 (_destPath는 resolver 별칭 허용: "save:/patch/ui.pak")
	//   사용 예) daemon.Http().DownloadAsync("https://cdn.example.com/patch/0001.pak", "patch:/0001.pak", nullptr,
	//                [](const IOResult& _r)
	//                {
	//                    if (_r.IsOk()) ApplyPatch();
	//                });
	IOResult Download(const String& _url, const String& _destPath, const HttpResponsePtr& _spResponse = nullptr);
	IOHandle DownloadAsync(const String& _url, const String& _destPath, const HttpResponsePtr& _spResponse = nullptr, const IOCallback& _callback = nullptr);

	// 저수준 공통 진입점 — Get/Post는 Send를 조립해 부르는 래퍼
	//   사용 예) HttpRequest req("https://api.example.com/v1/login");
	//            req.SetMethod(HttpMethod::hmPut)
	//               .SetHeader("X-Auth", token)
	//               .SetBody(spBody);
	//            IOResult r = daemon.Http().Send(req, spRes);
	IOResult Send(const HttpRequest& _request, const HttpResponsePtr& _spResponse);
	IOHandle SendAsync(const HttpRequest& _request, const HttpResponsePtr& _spResponse, const IOCallback& _callback = nullptr);

private:
	friend class IODaemon;
	HttpService(PathResolver& _resolver, IOEngine& _engine);
	bool Initialize(const HttpServiceConfig& _config, _s64 _memoryLimit, _s32 _readUnit);
	void Shutdown();

	// 메모리 전송 조립 — _spResponse의 body 스트림을 목적지로 (없으면 생성)
	IOSourcePtr MakeSource(const HttpRequest& _request, bool _failNon2xx, const HttpResponsePtr& _spResponse);
	IODestPtr MakeMemoryDest(const HttpResponsePtr& _spResponse);

	PathResolver& resolver_;
	IOEngine& engine_;
	IHttpTransport* pTransport_ = nullptr;		// Initialize에서 생성, Shutdown에서 파괴 (IOCP 교체 지점)
	HttpServiceConfig config_;
	_s64 memoryLimit_ = 256LL * 1024 * 1024;
	_s32 readUnit_ = 256 * 1024;
};

NS_END
