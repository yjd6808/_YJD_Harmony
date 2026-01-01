/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:21:54 AM
 * =====================
 *
 */


#pragma once

#include <jc/Type.h>
#include <jc/Define.h>
#include <jc/Encoding/CodePage.h>


NS_JC_BEGIN

class String;
class File
{
public:
	static bool Exist(const char* _pPath);
	static bool Exist(const String& _path);

	// 파일이 없으면 -1 반환
	static long Size(const char* _pPath);
	static long Size(const String& _path);

	static bool Delete(const char* _pPath);
	static bool Delete(const String& _path);

	static bool Move(const char* _pSrcPath, const char* _pDstPath);
	static bool Move(const String& _srcPath, const String& _dstPath);

	static bool Copy(const char* _pSrcPath, const char* _pDstPath);
	static bool Copy(const String& _srcPath, const String& _dstPath);

	// "abcd", 4 (NULL 문자는 뺀 길이를 전달해줘야함)
	static void WriteAllText(const char* _pContent, int _contentLength, const char* _pPath);
	static void WriteAllText(const String& _content, const char* _pPath);
	static void WriteAllBytes(const Byte* _pContent, int _contentLength, const char* _pPath);

	static String ReadAllText(const char* _pPath);

	template <Int32U ContentLength>
	static void WriteAllText(const char (&_content)[ContentLength], const char* _pPath)
	{
		// NULL 문자 뺀 길이를 전달
		WriteAllText(_content, ContentLength - 1, _pPath);
	}

private:
	static void FormatFileMode(char* _pModeBuffer, int _modeBufferCapacity, const char* _pDefaultMode, CodePage _codePage);
};


NS_JC_END
