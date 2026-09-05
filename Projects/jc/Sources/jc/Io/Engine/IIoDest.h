/*
 * 작성자: 윤정도
 * =====================
 */

#pragma once

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"
#include "jc/IO/IODefine.h"

NS_JC_BEGIN

class JC_NOVTABLE IIODest
{
public:
	virtual ~IIODest() = default;

	// 엔진이 핸들을 확정한 직후 호출 — FileDest의 .part.{handle} 이름 확정에 사용
	virtual void Bind(IOHandle _handle) { (void)_handle; }

	virtual bool Open(_s64 _expectedBytes) = 0;
	virtual bool Write(const _byte* _pBuffer, _s32 _len) = 0;
	virtual bool Commit() = 0;		// 성공 마감
	virtual void Abort() = 0;		// 실패/취소 마감
	virtual IOError GetLastError() const = 0;

	// 완료 결과 회수 — MemoryDest만 버퍼를 반환하고 나머지는 nullptr
	virtual MemoryStreamPtr TakeResult() { return nullptr; }
};

using IODestPtr = SharedPtr<IIODest>;

NS_END
