# jnet — WinHTTP 기반 HTTP 라이브러리

> 위치: `Projects/jnet/Sources/jnet/` | 빌드: `Scripts/BuildProject/jnet.bat -C Debug -P x64` | 링크: `Props/jnet.props` → `jnet.lib;ws2_32.lib;Mswsock.lib;winhttp.lib` (Windows SDK, 별도 ThirdParty 불필요)
> 의존성: `jc` (Uri/String/MemoryStream/ThreadPool)

## 모듈 개요

| 구분 | 파일 | 역할 |
|---|---|---|
| Define | `Http/HttpDefine.h` | `HttpMethod { hmGet, hmPost, hmPut, hmDelete, hmHead, hmPatch }`, `HttpState { hsNone..hsFailed }`, `HttpError { heNone..heShutdown }`, `HttpProgress`, `HttpClientConfig` |
| Headers | `Http/HttpHeaders.h/.cpp` | 대소문자 무시 헤더 맵 (`TreeMap`, 정규화=lower-case). `GetContentLength()` |
| Request | `Http/HttpRequest.h/.cpp` | 체이닝 빌더 — `SetMethod / SetHeader / SetBody / SetConnectTimeout / SetReceiveTimeout` |
| Transport | `Http/IHttpTransport.h`, `WinHttpTransport.h/.cpp` | `IHttpTransport::Open(Request) -> IHttpConnection`. `WinHttpTransport`는 `WinHttpOpen/Connect/OpenRequest/SendRequest/ReceiveResponse/ReadData` 래핑 |
| Response | `Http/HttpResponse.h/.cpp` | `statusCode`, `headers`, `body(MemoryStream)` 또는 스트리밍 `IHttpConnection` |
| Client | `Http/HttpClient.h/.cpp` | 동기(`Send/Get/Download`) + 비동기(`SendAsync/DownloadAsync`) + `Update / GetProgress / Cancel / GetLastSendError` |
| Bridge | `Http/HttpDataSource.h/.cpp` | `FileLoader`에 `usHttp/usHttps`를 `HttpClient`로 연결 — `InstallHttpDataSource(loader, client)` |

## HttpClientConfig

```cpp
struct HttpClientConfig {
    jc::String userAgent_ = "jnet-http/1.0"; // WinHttpOpen 첫 인자. 비면 기본값
    int connectTimeoutMs_ = 10000;
    int sendTimeoutMs_    = 10000;
    int receiveTimeoutMs_ = 30000;   // per-request SetReceiveTimeout으로 덮어쓰기 가능
    int maxRedirects_ = 5;           // WinHTTP WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS
    int workerCount_ = 2;
    int readUnit_  = 256*1024;
    int writeUnit_ = 256*1024;
    _s64 memoryLimit_ = 256*1024*1024;
    int progressNotifyIntervalMs_ = 100;
    jc::ThreadPool* pSharedPool_ = nullptr;
};
enum HttpCompletionOption { hcContentReceived, hcHeadersReceived }; // 스트리밍 옵션
```
