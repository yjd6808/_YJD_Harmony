#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * MemoryDest — 메모리 목적지 (호출자 소유 버퍼에 직접 기록)
 *   생성 시 호출자의 _spOut을 받아 그 위에 기록한다. 완료 후 회수 단계가 없다 (TakeResult 삭제).
 *   총량 검사가 소관 — memoryLimit_ 초과 시 ieMemoryLimit, 고정 버퍼 초과 시 ieBufferTooSmall.
 */

#include "jc/Namespace.h"
#include "jc/IO/MemoryStream.h"
#include "jc/IO/Engine/IIODest.h"

NS_JC_BEGIN

class MemoryDest final : public IIODest
{
public:
	MemoryDest(const MemoryStreamPtr& _spOut, _s64 _memoryLimit);
	~MemoryDest() override;

	bool Open(_s64 _expectedBytes) override;
	bool Write(const _byte* _pBuffer, _s32 _len) override;
	_byte* BeginWrite(_u32 _maxLen, OUT _u32& _outGranted) override;
	bool EndWrite(_u32 _written) override;
	bool Commit() override;
	void Abort() override;
	IOError GetLastError() const override { return lastError_; }
	_s32 GetChannelError() const override { return 0; }		// 메모리 목적지는 OS/전송 에러 없음
	IOType GetType() const override { return IOType::Memory; }

private:
	MemoryStreamPtr spBuffer_;		// 호출자 소유 — 생성/파괴하지 않음
	_s64 memoryLimit_;
	IOError lastError_ = ieNone;
};

NS_END
