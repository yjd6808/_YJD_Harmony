#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * MemorySource — 메모리 원천 (P1-2 Save용 신규)
 *   호출자 버퍼를 읽기 전용으로 소비 (자체 커서 — 스트림의 readOffset_을 건드리지 않음).
 *   BeginRead/EndRead 제로카피로 Memory→File Save가 중간 복사 없이 동작한다.
 */

#include "jc/Namespace.h"
#include "jc/Container/MemoryStream.h"
#include "jc/IO/Engine/IIOSource.h"

NS_JC_BEGIN

class MemorySource final : public IIOSource
{
public:
	explicit MemorySource(const MemoryStreamPtr& _spData);
	~MemorySource() override;

	bool Open(OUT IOSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IOError GetLastError() const override { return lastError_; }
	_s32 GetChannelError() const override { return 0; }		// 메모리 원천은 OS/전송 에러 없음
	IOType GetType() const override { return IOType::Memory; }

	const _byte* BeginRead(_u32& _outAvailable) override;
	void EndRead(_u32 _consumed) override;

private:
	MemoryStreamPtr spData_;
	_u32 offset_ = 0;				// 자체 읽기 커서 (스트림 공유 오프셋과 독립)
	IOError lastError_ = ieNone;
};

NS_END
