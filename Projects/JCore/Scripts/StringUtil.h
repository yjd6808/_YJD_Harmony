/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <vector>

namespace JCore {

class String;
class StringUtil final
{
public:
	static std::vector<String> Split(String& src, const char* delimiter);
	static String Format(const char* format, ...);
	static const int Length(const char* str);
	static const int Copy(char* buffer, const int bufferSize, const char* copy);
	static const bool IsEqual(const char* src, const int srcLen, const char* dst, const int dstLen);
	static void Swap(String& src, String& dst);
};

} // namespace JCore

