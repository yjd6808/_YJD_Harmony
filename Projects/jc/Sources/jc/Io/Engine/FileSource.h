#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/Io/Engine/IIoSource.h"

NS_JC_BEGIN

// 로컬 파일 원천 (구 FileDataSource 승계) — 접수 시점에 확정된 절대경로만 받는다
class FileSource final : public IIoSource
{
public:
	explicit FileSource(const String& _absPath);
	~FileSource() override;

	bool Open(OUT IoSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IoError GetLastError() const override { return lastError_; }
	IoErrorDetail GetLastErrorDetail() const override { return detail_; }

private:
	String path_;
	_iohandle hFile_ = nullptr;
	IoError lastError_ = ieNone;
	IoErrorDetail detail_;
};

NS_END
