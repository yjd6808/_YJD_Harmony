#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IIoDest — 바이트를 받는 쪽(목적지) 계약 (C안 데이터 평면, 구 Sink → Dest 명명, 04 문서 §3.6)
 *   FileDest는 .part.{handle} 에 쓰고 Commit 시 Move, Abort 시 Delete (원자적 커밋 규약)
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"
#include "jc/Io/IoDefine.h"

NS_JC_BEGIN

class JC_NOVTABLE IIoDest
{
public:
	virtual ~IIoDest() = default;

	// 엔진이 핸들을 확정한 직후 호출 — FileDest의 .part.{handle} 이름 확정에 사용
	virtual void Bind(IoHandle _handle) { (void)_handle; }

	virtual bool Open(_s64 _expectedBytes) = 0;
	virtual bool Write(const _byte* _pBuffer, _s32 _len) = 0;
	virtual bool Commit() = 0;		// 성공 마감
	virtual void Abort() = 0;		// 실패/취소 마감
	virtual IoError GetLastError() const = 0;

	// 완료 결과 회수 — MemoryDest만 버퍼를 반환하고 나머지는 nullptr
	virtual MemoryStreamPtr TakeResult() { return nullptr; }
};

using IoDestPtr = SharedPtr<IIoDest>;

NS_END
