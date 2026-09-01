#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IoJobEngine — 핸들·잡맵·큐·워커·펌프·취소 (구 FileLoader 기계 이식, B안 공용 엔진)
 *
 *   - Submit : Source/Dest 조립물을 접수 → 핸들 발급 → 워커 실행
 *   - RunSync: 접수 스레드에서 인라인 실행 (동기 API)
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
#include "jc/Io/IoDefine.h"
#include "jc/Io/IIoListener.h"
#include "jc/Io/Engine/TransferJob.h"

NS_JC_BEGIN

struct IoJobEngineConfig
{
	_s32 workerCount_ = 4;							// 단일 풀
	_s32 readUnit_ = 256 * 1024;
	_s32 progressNotifyIntervalMs_ = 100;
	_s32 completedProgressKeep_ = 256;				// 완료 진행률 링 캐시 상한 (R9)
	ThreadPool* pSharedPool_ = nullptr;
};

class IoJobEngine final : public NonCopyableNonMovable
{
public:
	IoJobEngine() = default;
	~IoJobEngine();

	bool Initialize(const IoJobEngineConfig& _config);
	void Shutdown();

	IoHandle Submit(const IoSourcePtr& _spSource, const IoDestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy,
		const IoCallback& _callback, bool _isHttp);
	IoHandle FailImmediate(IoError _error, const String& _uri, const IoCallback& _callback);

	IoResultPtr RunSync(const IoSourcePtr& _spSource, const IoDestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy, bool _isHttp);

	void SetListener(IIoListener* _pListener, bool _transferOwnership = false);

	// 메인 스레드 통지 펌프 — 진행 합삭 큐 + 완료 큐 소비
	void Pump();

	// 관제 (모든 창구 공통, 단일 핸들 공간)
	IoState GetState(IoHandle _handle) const;
	bool GetProgress(IoHandle _handle, OUT IoProgress& _out) const;
	bool Cancel(IoHandle _handle);
	void CancelAll();
	int GetActiveCount() const;
	void GetActiveSnapshot(OUT Vector<IoProgress>& _out) const;

	bool IsInitialized() const { return initialized_.Load(); }

private:
	TransferJobPtr MakeJob(const IoSourcePtr& _spSource, const IoDestPtr& _spDest,
		const String& _uri, const TransferPolicy& _policy, const IoCallback& _callback, bool _isHttp);
	void Finalize(const TransferJobPtr& _spJob);			// 결과 조립 + 완료 링 캐시 기록
	void PushProgress(const TransferJobPtr& _spJob);
	IoProgress MakeProgress(const TransferJobPtr& _spJob) const;
	void ExecuteOnWorker(const TransferJobPtr& _spJob);

	static void ExecuteJob(IoJobEngine* _pSelf, const TransferJobPtr& _spJob);

private:
	// [Config]
	IoJobEngineConfig config_;

	// [State]
	Atomic<bool> initialized_{ false };
	Atomic<bool> shuttingDown_{ false };
	IdProvider<IoHandle> idProvider_;					// 단일 핸들 공간

	// [Threading] 단일 ThreadPool + 워커 실행 카운터
	ThreadPool* pPool_ = nullptr;
	bool poolOwned_ = false;
	Atomic<int> runningWorkerJobs_{ 0 };

	// [Listener]
	IIoListener* pListener_ = nullptr;
	bool listenerOwned_ = false;

	// [Jobs] 활성 잡맵 (동기/비동기 모두 등록 — 비대칭 #4 해소)
	mutable NormalLock activeLock_;
	TreeMap<IoHandle, TransferJobPtr> activeJobs_;

	// [Queues] 진행(합삭) + 완료 큐 — 워커 → Pump
	NormalLock progressLock_;
	TreeMap<IoHandle, IoProgress> pendingProgress_;		// 핸들당 최신 1건 합삭 (R9/성능 4)
	NormalLock completedLock_;
	Vector<TransferJobPtr> completedQueue_;

	// [RingCache] 완료 후에도 GetProgress가 응답하는 유한 링 캐시 (무한 성장 맵 대체)
	mutable NormalLock ringLock_;
	Vector<IoProgress> ring_;
	int ringHead_ = 0;
};

NS_END
