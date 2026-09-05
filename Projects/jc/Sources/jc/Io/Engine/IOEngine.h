#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IOEngine — 핸들·잡맵·큐·워커·펌프·취소 (구 IOJobEngine, B안 공용 엔진)
 *
 *   - Submit : Source/Dest 조립물을 접수 → 핸들 발급 → 워커 실행
 *   - RunSync: 접수 스레드에서 인라인 실행 (동기 API) — IOResult 값 반환
 *   - Pump   : 메인 스레드 통지 펌프 (진행 합삭 + 완료 큐 swap 소비)
 *   - 관제   : GetState/GetProgress(완료 링 캐시)/Cancel/CancelAll/GetActiveCount/Snapshot
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Pool/IdProvider.h"
#include "jc/Sync/NormalLock.h"
#include "jc/Sync/LockGuard.h"
#include "jc/Container/Vector.h"
#include "jc/Container/TreeMap.h"
#include "jc/IO/Engine/IOTypes.h"
#include "jc/IO/Engine/IIOListener.h"
#include "jc/IO/Engine/IOJob.h"

NS_JC_BEGIN

struct IOEngineConfig
{
	_s32 workerCount_ = 4;							// 단일 풀
	_s32 readUnit_ = 256 * 1024;
	_s32 progressNotifyIntervalMs_ = 100;
	_s32 completedProgressKeep_ = 256;				// 완료 진행률 링 캐시 상한 (R9)
	ThreadPool* pSharedPool_ = nullptr;
};

class IOEngine final : public NonCopyableNonMovable
{
public:
	IOEngine() = default;
	~IOEngine();

	bool Initialize(const IOEngineConfig& _config);
	void Shutdown();

	IOHandle Submit(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy,
		const IOCallback& _callback);
	IOHandle FailImmediate(IOError _error, const String& _uri, const IOCallback& _callback);

	IOResult RunSync(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy);

	void SetListener(IIOListener* _pListener, bool _transferOwnership = false);

	// 메인 스레드 통지 펌프 — 진행 합삭 큐 + 완료 큐 소비
	void Pump();

	// 관제 (모든 창구 공통, 단일 핸들 공간)
	IOState GetState(IOHandle _handle) const;
	bool GetProgress(IOHandle _handle, OUT IOProgress& _out) const;
	bool Cancel(IOHandle _handle);
	void CancelAll();
	int GetActiveCount() const;
	void GetActiveSnapshot(OUT Vector<IOProgress>& _out) const;

	bool IsInitialized() const { return initialized_.Load(); }

private:
	IOJobPtr MakeJob(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy, const IOCallback& _callback);
	bool Finalize(const IOJobPtr& _spJob);				// 결과 조립 + 완료 링 캐시 기록 (선점 성공 시 true)
	void PushProgress(const IOJobPtr& _spJob);
	IOProgress MakeProgress(const IOJobPtr& _spJob) const;
	void ExecuteOnWorker(const IOJobPtr& _spJob);

	// 유일한 복사 루프 (C안 심장) — 3경로: 소스 직독 / 목적지 직접기록 / 스테이징 폴백
	//   _pWorkBuf == nullptr이면 폴백 경로에서만 lazy 할당
	void PumpJob(const IOJobPtr& _spJob, _byte* _pWorkBuf, _s32 _workBufLen);

	static void ExecuteJob(IOEngine* _pSelf, const IOJobPtr& _spJob);

private:
	// [Config]
	IOEngineConfig config_;

	// [State]
	Atomic<bool> initialized_{ false };
	Atomic<bool> shuttingDown_{ false };
	IdProvider<IOHandle> idProvider_;					// 단일 핸들 공간

	// [Threading] 단일 ThreadPool + 워커 실행 카운터
	ThreadPool* pPool_ = nullptr;
	bool poolOwned_ = false;
	Atomic<int> runningWorkerJobs_{ 0 };

	// [Listener]
	IIOListener* pListener_ = nullptr;
	bool listenerOwned_ = false;

	// [Jobs] 활성 잡맵 (동기/비동기 모두 등록 — 비대칭 #4 해소)
	mutable NormalLock activeLock_;
	TreeMap<IOHandle, IOJobPtr> activeJobs_;

	// [Queues] 진행(합삭) + 완료 큐 — 워커 → Pump
	NormalLock progressLock_;
	TreeMap<IOHandle, IOProgress> pendingProgress_;		// 핸들당 최신 1건 합삭 (R9/성능 4)
	NormalLock completedLock_;
	Vector<IOJobPtr> completedQueue_;

	// [RingCache] 완료 후에도 GetProgress가 응답하는 유한 링 캐시 (무한 성장 맵 대체)
	mutable NormalLock ringLock_;
	Vector<IOProgress> ring_;
	int ringHead_ = 0;
};

NS_END
