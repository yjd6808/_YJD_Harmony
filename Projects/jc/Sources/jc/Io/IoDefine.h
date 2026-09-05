#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * Io 통합 전송 계층 공통 정의
 *   - IOHandle / IOState(is*) / IOError(ie*) / IOErrorDetail
 *   - IOProgress / IOCallback / TransferPolicy / LoadOptions / IODaemonConfig
 */

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Functional.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/String.h"
#include "jc/Container/MemoryStream.h"
#include "jc/Threading/ThreadPool.h"

#include "jc/IO/Http/HttpTypes.h"

NS_JC_BEGIN

class IOResult;
using IOResultPtr = SharedPtr<IOResult>;

class IIOSource;
class IIODest;

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
	ieHttpStatusFailed,
	ieMemoryLimit,
	ieCancelledByUser,
	ieShutdown
};

// 에러 무손실(R4): 공통 코드 + 채널 상세 (HTTP 상태, TLS/타임아웃 등 전송 계층 코드)
struct IOErrorDetail
{
	_s32 channelError_ = 0;		// 채널 고유 코드 (예: HttpError)
	_s32 httpStatus_ = 0;		// HTTP 상태 코드 (HTTP가 아니면 0)
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
	_s64 memoryLimit_ = 256LL * 1024 * 1024;		// 메모리 목적지(MemoryDest) 총량 한도
	_s32 progressNotifyIntervalMs_ = 100;
	_s32 completedProgressKeep_ = 256;				// 완료 진행률 링 캐시 상한 (R9)
	ThreadPool* pSharedPool_ = nullptr;
	HttpServiceConfig http_;						// UserAgent / 타임아웃 / 리다이렉트
};

NS_END
