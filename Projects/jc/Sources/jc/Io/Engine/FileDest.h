#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/Io/Engine/IIoDest.h"

NS_JC_BEGIN

// 디스크 파일 목적지 (구 FileSink) — .part.{handle} 쓰기 → Commit 시 Move, Abort 시 Delete
class FileDest final : public IIoDest
{
public:
	explicit FileDest(const String& _destPath);
	~FileDest() override;

	void Bind(IoHandle _handle) override;
	bool Open(_s64 _expectedBytes) override;
	bool Write(const _byte* _pBuffer, _s32 _len) override;
	bool Commit() override;
	void Abort() override;
	IoError GetLastError() const override { return lastError_; }

private:
	String destPath_;
	String partPath_;
	_iohandle hFile_ = nullptr;
	bool opened_ = false;
	IoError lastError_ = ieNone;
};

NS_END
