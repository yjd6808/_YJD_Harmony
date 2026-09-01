# jc — JCore 기반 로더 라이브러리

> 위치: `Projects/jc/Sources/jc/` | 빌드: `Scripts/BuildProject/jc.bat -C Debug -P x64` | 산출물: `Output/x64/Debug/jc.lib`
> 의존성: 없음(jc 단독) — `FileLoader`가 `IFileDataSource` 팩토리로 파일 I/O를 추상화

## 모듈 개요

| 구분 | 파일 | 역할 |
|---|---|---|
| Uri | `Primitives/Uri.h/.cpp` | `file://`, `http://`, `https://`, 로컬 경로 파싱. `UriScheme { usUnknown, usFile, usHttp, usHttps }` |
| MemoryStream | `Container/MemoryStream.h/.cpp` | 소유/비소유 바이트 버퍼. `Write`→ 자동 확장(소유 시), `Read`→ 내부 offset |
| DataSource | `FileSystem/Loader/IFileDataSource.h`, `FileDataSource.h/.cpp` | `Open(Uri) -> Read/Close` 추상. 기본 등록은 `usFile` |
| Job/Result | `FileLoaderJob.h`, `FileLoaderResult.h/.cpp`, `FileLoaderDefine.h` | `LoadState { lsNone..lsFailed }`, `LoadError { leNone..leShutdown }`, `LoadProgress` |
| FileLoader | `FileSystem/Loader/FileLoader.h/.cpp` | 동기/비동기 로드·다운로드, 스레드풀, 큐, 리스너, 취소 |

## 핵심 타입

```cpp
enum UriScheme  { usUnknown, usFile, usHttp, usHttps };
enum LoadState  { lsNone, lsPending, lsOpening, lsLoading, lsCompleted, lsCancelled, lsFailed };
enum LoadError  { leNone, leInvalidUri, leUnsupportedScheme, leOpenFailed, leSourceFailed, leWriteFailed, leMemoryLimit, leCancelledByUser, leShutdown };

struct FileLoaderConfig {
    _s32 workerCount_ = 2;          // 내부 ThreadPool 크기
    _s32 readUnit_ = 256*1024;      // Read 단위
    _s32 writeUnit_ = 256*1024;     // Write 단위
    _s64 memoryLimit_ = 256*1024*1024; // 메모리 로드 상한
    _s32 progressNotifyIntervalMs_ = 100;
    ThreadPool* pSharedPool_ = nullptr; // 외부 공유 시 소유권 이전 안 함
};
using LoadHandle = _u32; // InvalidLoadHandle = 0
using LoadCallback = Action<LoadResult&>; // jc::Functional.h
```
