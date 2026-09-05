#pragma once

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/IO/Engine/IIOSource.h"

NS_JC_BEGIN

class FileSource final : public IIOSource
{
public:
	explicit FileSource(const String& _absPath);
	~FileSource() override;

	bool Open(OUT IOSourceInfo& _info) override;
	_s32 Read(_byte* _pBuffer, _s32 _len) override;
	void Close() override;
	IOError GetLastError() const override { return lastError_; }
	IOErrorDetail GetLastErrorDetail() const override { return detail_; }

private:
	String path_;
	_iohandle hFile_ = nullptr;
	IOError lastError_ = ieNone;
	IOErrorDetail detail_;
};

NS_END
