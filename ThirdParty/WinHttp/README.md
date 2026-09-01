# WinHttp

WinHTTP는 Windows SDK에서 제공하는 시스템 라이브러리이며 별도의 ThirdParty 바이너리가 필요하지 않습니다.

## 개요

- `jnet`의 1차 HTTP 전송 백엔드인 `WinHttpTransport`는 Windows SDK의 WinHTTP API를 사용합니다.
- 링크 라이브러리: `winhttp.lib` (`Props/jnet.props`의 `AdditionalDependencies`에 추가: `jnet.lib;ws2_32.lib;Mswsock.lib;winhttp.lib;%(AdditionalDependencies)`)
- 헤더: `winhttp.h` (`<Windows.h>` + `<winhttp.h>`, Windows SDK 제공)
- 사용 API: `WinHttpOpen` / `WinHttpConnect` / `WinHttpOpenRequest` / `WinHttpAddRequestHeaders` / `WinHttpSendRequest` / `WinHttpReceiveResponse` / `WinHttpQueryHeaders` / `WinHttpReadData` / `WinHttpCloseHandle` / `WinHttpSetTimeouts` / `WinHttpSetOption`

## ThirdParty 바이너리 불필요 사유

- WinHTTP는 OS(Windows SDK)에 포함된 네이티브 라이브러리로, 별도 소스/바이너리 배포 없이 SDK 링크만으로 빌드가 완료됩니다.
- 따라서 `ThirdParty/WinHttp` 디렉토리에는 `include/` / `lib/` 바이너리를 두지 않습니다.

## 구조 참고

- 본 디렉토리의 부재가 의도된 것임을 명시하기 위해 `README.md`만 유지합니다.
- 외부 라이브러리 배치 규칙은 `ThirdParty/Zlib12` (`include/` + `lib/` 분리), `ThirdParty/Bass24` 등의 구조를 참고했습니다.
- 향후 WinHTTP를 자체 소켓+schannel 기반 `SocketHttpTransport`로 교체하더라도 `IHttpTransport` 시임 덕분에 `HttpDataSource` / `HttpClient` 인터페이스는 변경되지 않습니다.
