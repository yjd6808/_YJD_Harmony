#pragma once

#include "jc/Namespace.h"
#include "jc/Container/MemoryStream.h"
#include "jc/IO/Engine/IIODest.h"

NS_JC_BEGIN

// 메모리 버퍼 목적지 (구 MemorySink) — 총량 검사 소관 (규약: 메모리 제한은 Dest가 검사)
class MemoryDest final : public IIODest
{
public:
	MemoryDest(_s64 _memoryLimit, _u32 _initCapacity = 256 * 1024);
	~MemoryDest() override;

	bool Open(_s64 _expectedBytes) override;
	bool Write(const _byte* _pBuffer, _s32 _len) override;
	bool Commit() override;
	void Abort() override;
	IOError GetLastError() const override { return lastError_; }
	MemoryStreamPtr TakeResult() override;

private:
	_s64 memoryLimit_;
	_u32 initCapacity_;
	MemoryStreamPtr spBuffer_;
	IOError lastError_ = ieNone;
};

NS_END
