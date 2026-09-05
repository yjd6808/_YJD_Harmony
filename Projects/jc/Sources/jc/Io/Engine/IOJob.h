#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IOJob — 잡 상태 덩어리 (구 TransferJob, B안 공용 엔진의 실행 단위)
 *   상태 전이는 CompareExchange (현행 FileLoader 검증 코드 보존).
 *   결과(IOResult 값)는 Finalize가 단 한 번만 조립한다 (resultReady_ 선점 — 취소/워커 이중 완료 방지).
 */

#include "jc/Namespace.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/String.h"
#include "jc/IO/Engine/IOTypes.h"
#include "jc/IO/Engine/IIOSource.h"
#include "jc/IO/Engine/IIODest.h"

NS_JC_BEGIN

struct IOJob
{
	// [Identity]
	IOHandle handle_ = InvalidIOHandle;
	String uri_;						// 표시용 원본 경로/URL
	TransferPolicy policy_;
	IOCallback callback_;

	// [Parts] 데이터 평면 부품 — 창구가 조립해 제출
	IOSourcePtr spSource_;
	IODestPtr spDest_;

	// [State] — 원자 상태 기계 (isPending → isOpening → isTransferring → 종료)
	Atomic<int> state_{ isPending };
	Atomic<int> error_{ ieNone };
	Atomic<_s64> totalBytes_{ -1 };
	Atomic<_s64> transferredBytes_{ 0 };
	Atomic<bool> cancelRequested_{ false };

	// [Result] — 워커가 채우고 엔진이 결과로 조립 (Finalize 선점 후 읽기 안전)
	_s32 chanError_ = 0;
	Atomic<bool> resultReady_{ false };	// Finalize 이중 실행 방지 (선점 성공한 쪽만 완료 큐 적재)
	IOResult result_;					// Finalize 산출물 (값 타입)

	bool TrySetState(int _from, int _to)
	{
		int expected = _from;
		return state_.CompareExchange(expected, _to);
	}

	bool IsTerminal() const
	{
		const int s = state_.Load();
		return s == isCompleted || s == isCancelled || s == isFailed;
	}

	// 스테이징 버퍼 필요 여부 — 어느 쪽도 제로카피를 지원하지 않을 때만 true
	//   (ExecuteJob/RunSync가 thread_local workbuf 확보 여부를 판단)
	bool NeedsStagingBuffer() const
	{
		_u32 avail = 0;
		if (spSource_ != nullptr && spSource_->BeginRead(avail) != nullptr)
		{
			spSource_->EndRead(0);
			return false;
		}
		_u32 granted = 0;
		if (spDest_ != nullptr && spDest_->BeginWrite(1, granted) != nullptr)
		{
			spDest_->EndWrite(0);
			return false;
		}
		return true;
	}
};

using IOJobPtr = SharedPtr<IOJob>;

NS_END
