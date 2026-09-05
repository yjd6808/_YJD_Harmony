#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/IO/Engine/IIOSource.h"

NS_JC_BEGIN

// 로컬 파일 원천 — 접수 시점에 확정된 절대경로 + 오프셋/카운트만 받는다 (P2-1 부분 읽기)
//   _count == 0 이면 끝까지
class FileSource final : public IIOSource
{
public:
	explicit FileSource(const String& _absPath, _s64 _offset = 0, _s64 _count = 0);
	~FileSource() override;

	bool Open(OUT IOSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IOError GetLastError() const override { return lastError_; }
	_s32 GetChannelError() const override { return channelError_; }
	IOType GetType() const override { return IOType::File; }

private:
	String path_;
	_s64 offset_ = 0;
	_s64 count_ = 0;
	_s64 remain_ = -1;
	_iohandle hFile_ = nullptr;
	IOError lastError_ = ieNone;
	_s32 channelError_ = 0;		// P0-2: OS 에러(errno) 기록 — R4 무손실
};

NS_END
