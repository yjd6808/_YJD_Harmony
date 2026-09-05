#include "jc/IO/File/FileDest.h"
#include "jc/IO/File.h"
#include "jc/IO/Directory.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
FileDest::FileDest(const String& _destPath)
	: destPath_(_destPath)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
FileDest::~FileDest()
{
	Abort();	// 미커밋 잔여 .part 정리 (Commit 성공 시에는 이미 닫혀 있음)
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileDest::Bind(IOHandle _handle)
{
	partPath_ = destPath_;
	partPath_.Append(".part.");
	partPath_.Append(StringUtil::ToString(_handle));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileDest::Open(_s64 _expectedBytes)
{
	(void)_expectedBytes;
	{
		// .part 파일의 부모 디렉토리 확보
		int slash = partPath_.FindReverse("/");
		int bslash = partPath_.FindReverse("\\");
		int pos = slash > bslash ? slash : bslash;
		if (pos > 0)
		{
			String parent = partPath_.SubStr(0, pos);
			if (!Directory::Exist(parent))
				Directory::Create(parent);
		}
	}
	hFile_ = CRuntime::FileOpen(partPath_.Source(), "wb");
	if (hFile_ == nullptr)
	{
		lastError_ = ieWriteFailed;
		channelError_ = CRuntime::ErrorNo();
		return false;
	}
	opened_ = true;
	lastError_ = ieNone;
	channelError_ = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileDest::Write(const _byte* _pBuffer, _s32 _len)
{
	if (!opened_)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	size_t written = CRuntime::FileWrite(_pBuffer, 1, _len, hFile_);
	if (written != (size_t)_len)
	{
		lastError_ = ieWriteFailed;
		channelError_ = CRuntime::ErrorNo();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileDest::Commit()
{
	if (hFile_ != nullptr)
	{
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
	}
	opened_ = false;

	if (!File::Move(partPath_, destPath_))
	{
		lastError_ = ieWriteFailed;
		channelError_ = CRuntime::ErrorNo();
		File::Delete(partPath_);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileDest::Abort()
{
	if (hFile_ != nullptr)
	{
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
	}
	opened_ = false;

	if (!partPath_.IsEmpty())
		File::Delete(partPath_);
}

NS_END
