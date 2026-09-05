#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IIODest — 바이트를 받는 쪽(목적지) 계약 (C안 데이터 평면, 구 Sink → Dest 명명, 04 문서 §3.6)
 *   FileDest는 .part.{handle} 에 쓰고 Commit 시 Move, Abort 시 Delete (원자적 커밋 규약)
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/IO/MemoryStream.h"		// _byte
#include "jc/IO/Engine/IOTypes.h"

NS_JC_BEGIN

class JC_NOVTABLE IIODest
{
public:
	virtual ~IIODest() = default;

	// 엔진이 핸들을 확정한 직후 호출 — FileDest의 .part.{handle} 이름 확정에 사용
	virtual void Bind(IOHandle _handle) { (void)_handle; }

	virtual bool Open(_s64 _expectedBytes) = 0;
	virtual bool Write(const _byte* _pBuffer, _s32 _len) = 0;

	// 제로카피 — 목적지 내부 버퍼 직접 노출. 미지원 목적지는 nullptr → 엔진이 스테이징 버퍼로 폴백
	//   _outGranted: 실제 허가량 (<= _maxLen, non-owner 고정 버퍼는 잔여분만) — 펌프는 허가량까지만 Read
	//   사용 예) — 엔진 펌프 내부
	//     _u32 granted = 0;
	//     if (_byte* p = dest.BeginWrite(readUnit, granted))
	//     {
	//         _s32 n = source.Read(p, granted);		// 중간 복사 없이 직접 기록
	//         dest.EndWrite((_u32)n);
	//     }
	virtual _byte* BeginWrite(_u32 _maxLen, OUT _u32& _outGranted) { (void)_maxLen; _outGranted = 0; return nullptr; }
	virtual bool EndWrite(_u32 _written) { (void)_written; return false; }

	virtual bool Commit() = 0;		// 성공 마감
	virtual void Abort() = 0;		// 실패/취소 마감
	virtual IOError GetLastError() const = 0;
	virtual _s32 GetChannelError() const = 0;	// 채널 원시 코드 (파일=OS 에러)
	virtual IOType GetType() const = 0;
};

using IODestPtr = SharedPtr<IIODest>;

NS_END
