/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:24:47 AM
 * =====================
 *
 */


#include "jc/Exception.h"

#include "jc/FileSystem/File.h"
#include "jc/Primitives/String.h"
#include "jc/Wrapper/CRuntime.h"

#include "jc/Primitives/StringUtil.h"

#define MODE_BUFFER_SIZE		32
#define FILE_READ_COUNT			64
#define READ_BUFFER_SIZE		1024

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Exist(const char* _pPath)
{
	_iohandle pHandle = CRuntime::FileOpen(_pPath, "rb");
	bool exists = false;

	if (pHandle)
	{
		exists = true;
		CRuntime::FileClose(pHandle);
	}

	return exists;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Exist(const String& _path)
{
	return Exist(_path.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
long File::Size(const char* _pPath)
{
	_iohandle pHandle = CRuntime::FileOpen(_pPath, "rb");
	long size = -1;

	if (pHandle && CRuntime::FileSeekEnd(pHandle, 0))
	{
		size = CRuntime::FileTell(pHandle);
		CRuntime::FileClose(pHandle);
	}

	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
long File::Size(const String& _path)
{
	return Size(_path.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Delete(const char* _pPath)
{
	return CRuntime::FileDelete(_pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Delete(const String& _path)
{
	return Delete(_path.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Move(const char* _pSrcPath, const char* _pDstPath)
{
	return CRuntime::FileRename(_pSrcPath, _pDstPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Move(const String& _srcPath, const String& _dstPath)
{
	return Move(_srcPath.Source(), _dstPath.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Copy(const char* _pSrcPath, const char* _pDstPath)
{
	_iohandle pSrcStream = CRuntime::FileOpen(_pSrcPath, "rb");

	if (pSrcStream == nullptr)
	{
		return false;
	}

	_iohandle pDstStream = CRuntime::FileOpen(_pDstPath, "wb");

	if (pDstStream == nullptr)
	{
		return false;
	}

	_u8 buff[FILE_READ_COUNT];
	size_t readCount = 0;

	while ((readCount = CRuntime::FileRead(buff, 1, FILE_READ_COUNT, pSrcStream)) != 0)
	{
		if (CRuntime::FileWrite(buff, readCount, 1, pDstStream) != 1)
		{
			return false;
		}
	}

	CRuntime::FileClose(pSrcStream);
	CRuntime::FileClose(pDstStream);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool File::Copy(const String& _srcPath, const String& _dstPath)
{
	return Copy(_srcPath.Source(), _dstPath.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::FormatFileMode(char* _pModeBuffer, const int _modeBufferCapacity, const char* _pDefaultMode, CodePage _codePage)
{
	switch (_codePage)
	{
	case CodePage::ANSI:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s", _pDefaultMode);
		break;
	case CodePage::UTF8:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s, ccs=UTF-8", _pDefaultMode);
		break;
	case CodePage::UTF16LE:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s, ccs=UTF-16LE", _pDefaultMode);
		break;

	// 아래 3개는 문서에 없다.
	case CodePage::UTF16BE:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s, ccs=UTF-16BE", _pDefaultMode);
		break;
	case CodePage::UTF32LE:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s, ccs=UTF-32LE", _pDefaultMode);
		break;
	case CodePage::UTF32BE:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s, ccs=UTF-32BE", _pDefaultMode);
		break;

	default:
		StringUtil::FormatBuffer(_pModeBuffer, _modeBufferCapacity, "%s", _pDefaultMode);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::WriteAllText(const char* _pContent, const int _contentLength, const char* _pPath)
{
	char mode[MODE_BUFFER_SIZE];
	FormatFileMode(mode, MODE_BUFFER_SIZE, "w", CodePage::ANSI);
	_iohandle pStream = CRuntime::FileOpen(_pPath, mode);

	if (pStream == nullptr)
	{
		throw RuntimeException("해당 파일이 이미 쓰기 모드로 사용중인듯?");
	}

	size_t writeCount = CRuntime::FileWrite(_pContent, _contentLength, 1, pStream);

	if (writeCount != 1)
	{
		throw RuntimeException("파일에 내용을 쓰는데 실패하였습니다.");
	}

	CRuntime::FileClose(pStream);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::WriteAllText(const String& _content, const char* _pPath)
{
	WriteAllText(_content.Source(), _content.Length(), _pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::WriteAllBytes(const _u8* _pContent, const int _contentLength, const char* _pPath)
{
	_iohandle pStream = CRuntime::FileOpen(_pPath, "wb");

	if (pStream == nullptr)
	{
		throw RuntimeException("해당 파일이 이미 쓰기 모드로 사용중인듯?");
	}

	size_t writeCount = CRuntime::FileWrite(_pContent, _contentLength, 1, pStream);

	if (writeCount != 1)
	{
		throw RuntimeException("파일에 내용을 쓰는데 실패하였습니다.");
	}

	CRuntime::FileClose(pStream);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String File::ReadAllText(const char* _pPath)
{
	char modeBuffer[MODE_BUFFER_SIZE];
	FormatFileMode(modeBuffer, MODE_BUFFER_SIZE, "r", CodePage::ANSI);
	_iohandle pStream = CRuntime::FileOpen(_pPath, modeBuffer);

	if (pStream == nullptr)
	{
		throw RuntimeException("해당 파일이 없거나 이미 쓰기 모드로 사용중인듯?");
	}

	const int fileSize = Size(_pPath);

	if (fileSize == -1)
	{
		throw RuntimeException("파일 사이즈 획득 실패");
	}

	String buffer(fileSize + 32);
	int readCount;
	int offset = 0;

	while ((readCount = (int)CRuntime::FileRead(buffer.Source() + offset, 1, FILE_READ_COUNT, pStream)) != 0)
	{
		offset += readCount;
		int nextLength = offset + FILE_READ_COUNT + 1; // 확장시 NULL 문자 고려해야함

		buffer.SetLength(offset);
		buffer.ResizeIfNeeded(nextLength);
	}

	buffer.Source()[offset] = '\0';

	CRuntime::FileClose(pStream);
	return buffer;
}

NS_END
