/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:21:54 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>
#include <JCore/Encoding/CodePage.h>


NS_JC_BEGIN

class String;
class File
{
public:
	static bool Exist(const char* path);
	static bool Exist(const String& path);

	// 파일이 없으면 -1 반환
	static long Size(const char* path);
	static long Size(const String& path);

	static bool Delete(const char* path);
	static bool Delete(const String& path);

	static bool Move(const char* srcPath, const char* dstPath);
	static bool Move(const String& srcPath, const String& dstPath);

	static bool Copy(const char* srcPath, const char* dstPath);
	static bool Copy(const String& srcPath, const String& dstPath);

	// "abcd", 4 (NULL 문자는 뺀 길이를 전달해줘야함)
	static void WriteAllText(const char* content, const int contentLength, const char* path);
	static void WriteAllText(const String& content, const char* path);
	static void WriteAllBytes(const Byte* content, const int contentLength, const char* path);


	static String ReadAllText(const char* path);


	template <Int32U ContentLength>
	static void WriteAllText(const char(&content)[ContentLength], const char* path) {
		// NULL 문자 뺀 길이를 전달
		WriteAllText(content, ContentLength - 1, path);
	}

private:
	static void FormatFileMode(char* modeBuffer, const int modeBufferCapacity, const char* dfaultMode, CodePage codePage);
};


NS_JC_END
