#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IO 공용 타입 모음 — 최하위 계약 계층 (IoDefine.h + IoResult.h 병합)
 *   - IOHandle / IOState(is*) / IOError(ie*) / IOType
 *   - IOResult(값 타입 — 데이터는 호출자 그릇, 여기엔 수행 결과만)
 *   - IOProgress / IOCallback / TransferPolicy / LoadOptions / IODaemonConfig
 *
 * 사용 예:
 *   MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
 *   IOResult r = daemon.Files().Load("save:/slot1.dat", spOut);
 *   if (r.IsOk())
 *   {
 *       Parse(spOut->GetData(), spOut->GetSize());
 *   }
 */

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Functional.h"
#include "jc/Primitives/String.h"

#include "jc/IO/Http/HttpTypes.h"

NS_JC_BEGIN

class ThreadPool;

using IOHandle = _u32;
constexpr IOHandle InvalidIOHandle = 0;

enum IOState
{
	isNone,
	isPending,
	isOpening,
	isTransferring,
	isCompleted,
	isCancelled,
	isFailed
};

enum IOError
{
	ieNone,
	ieInvalidUri,
	ieUnsupportedScheme,
	ieOpenFailed,
	ieReadFailed,
	ieWriteFailed,
	ieBufferTooSmall,		// 고정 버퍼(raw/non-owner) 용량 부족 — bytesTransferred_로 재시도 힌트
	ieHttpStatusFailed,
	ieMemoryLimit,
	ieCancelledByUser,
	ieShutdown
};

// 원천/목적지 종류 — 엔진이 구체 타입을 모르게 하는 식별자
enum class IOType
{
	File,
	Memory,
	Http
};

// 순수 수행 결과 (값 타입 — 복사 가능, heap 할당 불필요)
//   결과 데이터의 행선지: 파일/메모리 Load는 호출자의 _spOut, HTTP는 호출자의 _spResponse
struct IOResult
{
	IOHandle handle_ = InvalidIOHandle;
	IOState state_ = isNone;
	IOError error_ = ieNone;
	_s32 channelError_ = 0;			// 채널 원시 코드 — 파일=OS 에러(errno/GetLastError), HTTP=WinHTTP 에러. 0 = 없음
	_u64 bytesTransferred_ = 0;		// 실제 기록/전송된 바이트 수

	bool IsOk() const { return error_ == ieNone; }
};

struct IOProgress
{
	IOHandle handle_ = InvalidIOHandle;
	IOState state_ = isNone;
	_s64 totalBytes_ = -1;			// 미상 = -1
	_s64 transferredBytes_ = 0;
	String uri_;					// 로그/디버그 표시용 원본 경로 또는 URL
};

using IOCallback = Action<const IOResult&>;

struct TransferPolicy
{
	_s32 readUnit_ = 0;				// 0 = 데몬 기본값
	_s32 progressIntervalMs_ = 0;	// 0 = 데몬 기본값
};

struct LoadOptions
{
	String baseAlias_;				// 이 요청만 다른 마운트 별칭을 쓸 때 (요청 단위 오버라이드)
	TransferPolicy policy_;
};

struct IODaemonConfig
{
	_s32 workerCount_ = 4;							// 단일 ThreadPool (기존 파일용+HTTP용 2+2 대체)
	_s32 readUnit_ = 256 * 1024;
	_s64 memoryLimit_ = 256LL * 1024 * 1024;		// 성장형 스트림 안전 상한 (MemoryDest 총량 한도)
	_s32 progressNotifyIntervalMs_ = 100;
	_s32 completedProgressKeep_ = 256;				// 완료 진행률 링 캐시 상한 (R9)
	ThreadPool* pSharedPool_ = nullptr;
	HttpServiceConfig http_;						// UserAgent / 타임아웃 / 리다이렉트
};

NS_END
