#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/IO/Engine/IIODest.h"

NS_JC_BEGIN

class FileDest final : public IIODest
{
public:
	explicit FileDest(const String& _destPath);
	~FileDest() override;

	void Bind(IOHandle _handle) override;
	bool Open(_s64 _expectedBytes) override;
	bool Write(const _byte* _pBuffer, _s32 _len) override;
	bool Commit() override;
	void Abort() override;
	IOError GetLastError() const override { return lastError_; }

private:
	String destPath_;
	String partPath_;
	_iohandle hFile_ = nullptr;
	bool opened_ = false;
	IOError lastError_ = ieNone;
};

NS_END
