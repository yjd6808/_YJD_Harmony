#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IIOSource — 바이트를 내놓는 쪽 계약 (C안 데이터 평면)
 *   요청 정보(URL/헤더/경로/오프셋)는 생성자에서 주입받고, Open 성공 시 메타를 채운다.
 *   Read: >0 읽은 바이트, 0 = EOF, <0 = 에러
 *   BeginRead/EndRead: 제로카피 — 소스 내부 버퍼 직접 노출 (Memory→File Save 경로). 미지원은 nullptr
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"		// _byte
#include "jc/IO/IOTypes.h"

NS_JC_BEGIN

struct IOSourceInfo
{
	_s64 totalBytes_ = -1;					// 미상 = -1
};

class JC_NOVTABLE IIOSource
{
public:
	virtual ~IIOSource() = default;

	virtual bool Open(OUT IOSourceInfo& _info) = 0;
	virtual _s32 Read(_byte* _pBuffer, _s32 _len) = 0;
	virtual void Close() = 0;
	virtual IOError GetLastError() const = 0;
	virtual _s32 GetChannelError() const = 0;	// 채널 원시 코드 (파일=OS 에러, HTTP=WinHTTP 에러)
	virtual IOType GetType() const = 0;

	// 제로카피 — 소스 내부 버퍼 직접 노출. 미지원 소스는 nullptr
	//   사용 예) — 엔진 펌프 내부
	//     _u32 avail = 0;
	//     if (const _byte* p = source.BeginRead(avail))
	//     {
	//         dest.Write(p, min(avail, unit));
	//         source.EndRead(consumed);
	//     }
	virtual const _byte* BeginRead(_u32& _outAvailable) { (void)_outAvailable; return nullptr; }
	virtual void EndRead(_u32 _consumed) { (void)_consumed; }
};

using IOSourcePtr = SharedPtr<IIOSource>;

NS_END
