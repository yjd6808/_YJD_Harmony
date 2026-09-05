#include "jc/IO/File/FileSource.h"
#include "jc/IO/File.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
FileSource::FileSource(const String& _absPath, _s64 _offset, _s64 _count)
	: path_(_absPath)
	, offset_(_offset < 0 ? 0 : _offset)
	, count_(_count < 0 ? 0 : _count)
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
	hFile_ = CRuntime::FileOpen(path_.Source(), "rb");
	if (hFile_ == nullptr)
	{
		lastError_ = ieOpenFailed;
		channelError_ = CRuntime::ErrorNo();
		return false;
	}

	// P0-6: 64비트 탐색으로 파일 크기 확정 (2GB 한계 해소)
	if (!CRuntime::FileSeekEnd64(hFile_, 0))
	{
		lastError_ = ieReadFailed;
		channelError_ = CRuntime::ErrorNo();
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
		return false;
	}

	const _s64 fileSize = CRuntime::FileTell64(hFile_);
	if (fileSize < 0)
	{
		lastError_ = ieReadFailed;
		channelError_ = CRuntime::ErrorNo();
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
		return false;
	}

	_s64 start = offset_ > fileSize ? fileSize : offset_;
	if (!CRuntime::FileSeekBegin64(hFile_, start))
	{
		lastError_ = ieReadFailed;
		channelError_ = CRuntime::ErrorNo();
		CRuntime::FileClose(hFile_);
		hFile_ = nullptr;
		return false;
	}

	remain_ = fileSize - start;
	if (count_ > 0 && count_ < remain_)
		remain_ = count_;

	_info.totalBytes_ = remain_;
	lastError_ = ieNone;
	channelError_ = 0;
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
	if (remain_ <= 0)
		return 0;	// EOF (오프셋/카운트 범위 종료 포함)

	_s32 want = _len;
	if ((_s64)want > remain_)
		want = (_s32)remain_;

	size_t readCount = CRuntime::FileRead(_pBuffer, 1, want, hFile_);
	if (readCount == 0)
	{
		if (CRuntime::FileHasError(hFile_))
		{
			lastError_ = ieReadFailed;
			channelError_ = CRuntime::ErrorNo();
			return -1;
		}
		remain_ = 0;
		return 0;	// EOF
	}

	remain_ -= (_s64)readCount;
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
