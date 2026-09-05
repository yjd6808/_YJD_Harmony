#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IOJob — 잡 상태 덩어리 + Pump (유일한 복사 루프, C안 심장)
 *
 *   src.Open → dest.Open → loop{ 취소/셧다운 검사 → src.Read → dest.Write → 진행 push }
 *             → 성공: dest.Commit / 실패·취소: dest.Abort
 *   상태 전이는 CompareExchange (현행 FileLoader 검증 코드 보존).
 *   워커 전용 멤버(chanError_/sourceInfo_/spMemoryResult_)는 Pump 종료 후 엔진이 결과로 조립한다.
 */

#include "jc/Namespace.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/String.h"
#include "jc/IO/IODefine.h"
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
	bool isHttp_ = false;				// 결과의 http_ 부분 채움 여부

	// [Parts] 데이터 평면 부품 — 창구가 조립해 제출
	IOSourcePtr spSource_;
	IODestPtr spDest_;

	// [State] — 원자 상태 기계 (isPending → isOpening → isTransferring → 종료)
	Atomic<int> state_{ isPending };
	Atomic<int> error_{ ieNone };
	Atomic<_s64> totalBytes_{ -1 };
	Atomic<_s64> transferredBytes_{ 0 };
	Atomic<bool> cancelRequested_{ false };

	// [Data] — 워커가 채우고 엔진이 결과로 조립 (Pump 종료 후 읽기 안전)
	_s32 chanError_ = 0;
	IOSourceInfo sourceInfo_;
	MemoryStreamPtr spMemoryResult_;
	IOResultPtr spResult_;					// Finalize 산출물

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
};

using IOJobPtr = SharedPtr<IOJob>;

// 유일한 복사 루프 (C안 심장) — 재사용 버퍼 주입(잡당 할당 0회), _shutdown은 엔진 소유 플래그
void PumpTransferJob(const IOJobPtr& _spJob, _byte* _pWorkBuf, _s32 _workBufLen, const Atomic<bool>& _shutdown);

NS_END
