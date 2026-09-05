#include "jc/IO/File/FileSource.h"
#include "jc/IO/File.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
FileSource::FileSource(const String& _absPath)
	: path_(_absPath)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
FileSource::~FileSource()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileSource::Open(OUT IOSourceInfo& _info)
{
	if (!File::Exist(path_))
	{
		lastError_ = ieOpenFailed;
		detail_ = IOErrorDetail{};
		return false;
	}

	hFile_ = CRuntime::FileOpen(path_.Source(), "rb");
	if (hFile_ == nullptr)
	{
		lastError_ = ieOpenFailed;
		return false;
	}

	if (!CRuntime::FileSeekEnd(hFile_, 0))
	{
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
		lastError_ = ieReadFailed;
		return false;
	}

	long len = CRuntime::FileTell(hFile_);
	CRuntime::FileSeekBegin(hFile_, 0);

	_info.totalBytes_ = len >= 0 ? (_s64)len : -1;
	_info.httpStatus_ = 0;
	lastError_ = ieNone;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 FileSource::Read(_byte* _pBuffer, _s32 _len)
{
	if (hFile_ == nullptr)
	{
		lastError_ = ieReadFailed;
		return -1;
	}

	size_t readCount = CRuntime::FileRead(_pBuffer, 1, _len, hFile_);
	if (readCount == 0)
	{
		if (CRuntime::FileHasError(hFile_))
		{
			lastError_ = ieReadFailed;
			return -1;
		}
		return 0;	// EOF
	}

	return (_s32)readCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileSource::Close()
{
	if (hFile_ != nullptr)
	{
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
	}
}

NS_END
