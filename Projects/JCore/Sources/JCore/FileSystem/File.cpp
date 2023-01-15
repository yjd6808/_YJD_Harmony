/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:24:47 AM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Exception.h>

#include <JCore/FileSystem/File.h>
#include <JCore/Primitives/String.h>
#include <JCore/Wrapper/CRuntime.h>

#include <JCore/Primitives/StringUtil.h>

#define ModeBufferSize		32
#define FileReadCount		64
#define ReadBufferSize		1024

namespace JCore {

	bool File::Exist(const char* path) {
 		IoHandle hHandle = CRuntime::FileOpen(path, "rb");
		bool bRet = false;

		if (hHandle) {
			bRet = true;
			CRuntime::FileClose(hHandle);
		}

		return bRet;
	}
	bool File::Exist(const String& path) {
		return Exist(path.Source());
	}

	long File::Size(const char* path) {
		IoHandle hHandle = CRuntime::FileOpen(path, "rb");
		long iSize = -1;

		if (hHandle && CRuntime::FileSeekEnd(hHandle, 0)) {
			iSize = CRuntime::FileTell(hHandle);
			CRuntime::FileClose(hHandle);
		}

		return iSize;
	}

	long File::Size(const String& path) {
		return Size(path.Source());
	}

	bool File::Delete(const char* path) {
		return CRuntime::FileDelete(path);
	}

	bool File::Delete(const String& path) {
		return Delete(path.Source());
	}

	bool File::Move(const char* srcPath, const char* dstPath) {
		return CRuntime::FileRename(srcPath, dstPath);
	}

	bool File::Move(const String& src, const String& dstPath) {
		return Move(src.Source(), dstPath.Source());
	}

	bool File::Copy(const char* srcPath, const char* dstPath) {
		IoHandle hSrcStream = CRuntime::FileOpen(srcPath, "rb");

		if (hSrcStream == nullptr)
			return false;

		IoHandle hDstStream = CRuntime::FileOpen(dstPath, "wb");

		if (hDstStream == nullptr)
			return false;

		Byte buff[FileReadCount];
		int iReadCount = 0;
		while ((iReadCount = CRuntime::FileRead(buff, 1, FileReadCount, hSrcStream)) != 0) {
			if (CRuntime::FileWrite(buff, iReadCount, 1, hDstStream) != 1)
				return false;
		}

		CRuntime::FileClose(hSrcStream);
		CRuntime::FileClose(hDstStream);
		return true;
	}

	bool File::Copy(const String& srcPath, const String& dstPath) {
		return Copy(srcPath.Source(), dstPath.Source());
	}

	void File::FormatFileMode(char* modeBuffer, const int modeBufferCapacity, const char* defaultMode, CodePage codePage) {
		switch (codePage) {
		case CodePage::ANSI: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s", defaultMode);					break;
		case CodePage::UTF8: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s, ccs=UTF-8", defaultMode);		break;
		case CodePage::UTF16LE: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s, ccs=UTF-16LE", defaultMode);	break;

		// 아래 3개는 문서에 없다.
		case CodePage::UTF16BE: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s, ccs=UTF-16BE", defaultMode);	break;
		case CodePage::UTF32LE: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s, ccs=UTF-32LE", defaultMode);	break;
		case CodePage::UTF32BE: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s, ccs=UTF-32BE", defaultMode);	break;

		default: StringUtil::FormatBuffer(modeBuffer, modeBufferCapacity, "%s", defaultMode);
		}

	}

	// 인코딩 기능은 나중에 필요하면 구현하자.
	void File::WriteAllText(const char* content, const int contentLength, const char* path) {
		char mode[ModeBufferSize];
		FormatFileMode(mode, ModeBufferSize, "w", CodePage::ANSI);
		IoHandle hStream = CRuntime::FileOpen(path, mode);
		if (hStream == nullptr) throw RuntimeException("해당 파일이 이미 쓰기 모드로 사용중인듯?");
		int iWriteCount = CRuntime::FileWrite(content, contentLength, 1, hStream);
		if (iWriteCount != 1) throw RuntimeException("파일에 내용을 쓰는데 실패하였습니다.");
		CRuntime::FileClose(hStream);
	}

	void File::WriteAllText(const String& content, const char* path) {
		WriteAllText(content.Source(), content.Length(), path);
	}

	

	void File::WriteAllBytes(const Byte* content, const int contentLength, const char* path) {
		IoHandle hStream = CRuntime::FileOpen(path, "wb");
		if (hStream == nullptr) throw RuntimeException("해당 파일이 이미 쓰기 모드로 사용중인듯?");
		int iWriteCount = CRuntime::FileWrite(content, contentLength, 1, hStream);
		if (iWriteCount != 1) throw RuntimeException("파일에 내용을 쓰는데 실패하였습니다.");
		CRuntime::FileClose(hStream);
	}

	

	String File::ReadAllText(const char* path) {
		char szMode[ModeBufferSize];
		FormatFileMode(szMode, ModeBufferSize, "r", CodePage::ANSI);
		IoHandle hStream = CRuntime::FileOpen(path, szMode);
		if (hStream == nullptr) throw RuntimeException("해당 파일이 없거나 이미 쓰기 모드로 사용중인듯?");

		String szBuff(ReadBufferSize);
		int iReadCount;
		int iOffset = 0;
		while ((iReadCount = CRuntime::FileRead(szBuff.Source() + iOffset, 1, FileReadCount, hStream)) != 0) {
			iOffset += iReadCount;
			int iNextLen = iOffset + FileReadCount + 1;	// 확장시 NULL 문자 고려해야함

			szBuff.SetLength(iOffset);
			szBuff.ResizeIfNeeded(iNextLen);
		}

		CRuntime::FileClose(hStream);
		return szBuff;
	}

} // namespace JCore