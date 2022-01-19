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

	// 컴파일 타임용

	static constexpr int CTLength(const char* str) {
		return CTLengthRecursive(str, 0);
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 0번 인덱스부터 올라가면서 하나씩 검사
	static constexpr int CTFindChar(const char* str, const char ch) {
		return CTFindCharRecursive(str, ch, 0);
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 마지막 인덱스부터 내려가면서 하나씩 검사
	static constexpr int CTFindCharReverse(const char* str, const char ch) {
		const int strLen = CTLength(str);
		return CTFindCharReverseRecursive(str + strLen - 1, ch, strLen);
	}
	
private:
	static constexpr int CTLengthRecursive(const char* str, const int position) {
		return *str != NULL ? CTLengthRecursive(str + 1, position + 1) : position;
	}

	static constexpr int CTFindCharRecursive(const char* str, const char ch, const int position) {

		if (*str == NULL) {
			return -1;
		}

		if (*str == ch) {
			return position;
		}

		return CTFindCharRecursive(str + 1, ch, position + 1);
	}

	static constexpr int CTFindCharReverseRecursive(const char* str, const char ch, const int position) {

		if (position == 0) {
			return -1;
		}

		// 인덱스로 반환하자.
		if (*str == ch) {
			return position - 1;
		}

		return CTFindCharReverseRecursive(str - 1, ch, position - 1);
	}
};

} // namespace JCore

