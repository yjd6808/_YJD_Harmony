/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Namespace.h>

NS_JC_BEGIN

class DefaultAllocator;

template <typename, typename>
class Vector;
class String;
class StringUtil final
{
public:
	static Vector<String, DefaultAllocator> Split(String& src, const char* delimiter);
	static String Format(const char* format, ...);
	static String Format(const char* format, va_list args);
	static void FormatBuffer(char* buff, const int buffCapacity, const char* format, ...);
	static void FormatBuffer(char* buff, const int buffCapacity, const char* format, va_list va);

	template <typename TInteger>
	static TInteger ToNumber(const char* str, bool ignoreLeadingZero = true);

	static const char* SkipLeadingChar(const char* str, char skipChar);

	template <typename TInteger>
	static String ToString(TInteger integer);

	static int Length(const char* str);
	static int Copy(char* buffer, const int bufferSize, const char* copy);
	static int CopyUnsafe(char* buffer, const char* copy);
	static bool IsEqual(const char* src, const int srcLen, const char* dst, const int dstLen);
	static void Swap(String& src, String& dst);
	static int Find(const char* source, int sourceLen, int startIdx, int endIdx, const char* str);
	static int Find(const char* source, int sourceLen, int startIdx, const char* str);
	static int FindChar(const char* source, char ch);
	static int FindCharUncontained(const char* source, char ch);
	static String GetRange(const char* source, int sourceLen, int startIdx, int endIdx);
	static Tuple<char*, int, int> GetRangeUnsafe(const char* source, int sourceLen, int startIdx, int endIdx);

	// buf에 str 문자열 추가함
	// 반환 결과 합쳐진 문자열의 길이
	static void ConcatInnerBack(char* buf, int buflen, int bufCapacity, const char* concatStr, int concatStrLen);
	static void ConcatInnerBack(char* buf, int bufCapacity, const char* concatStr);

	static void ConcatInnerFront(char* buf, int buflen, int bufCapacity, const char* concatStr, int concatStrLen);
	static void ConcatInnerFront(char* buf, int bufCapacity, const char* concatStr);

	// 컴파일 타임용
	static constexpr int CTLength(const char* str) {
		return CTLengthRecursive(str, 0);
	}

	// 컴파일 타임용
	static constexpr int CTLength2(const char* str) {
		char* pStr = (char*)str;
		int iLength = 0;

		while (*pStr != '\0') {
			iLength++;
			pStr++;
		}

		return iLength;
	}

	template <Int32U Len>
	static constexpr int CTLength(const char(&str)[Len]) {
		return Len;
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 0번 인덱스부터 올라가면서 하나씩 검사
	static constexpr int CTCountChar(const char* str, const char ch) {
		return CTCountCharRecursive(str, ch, 0, 0);
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

	template <typename T, typename U>
	static constexpr int CTCompare(T&& src, U&& dst) {
		const int iSrcLen = CTLength(src);
		const int iDstLen = CTLength(dst);

		char* pSrc = (char*)src;
		char* pDst = (char*)dst;

		while (*pDst != NULL && *pSrc != NULL) {
			if (*pDst > *pSrc)
				return -1;
			else if (*pDst < *pSrc)
				return 1;

			++pDst;
			++pSrc;
		}

		if (iDstLen > iSrcLen)
			return -1;
		else if (iDstLen < iSrcLen)
			return 1;

		return 0;
	}
private:
	static constexpr int CTLengthRecursive(const char* str, const int position) {
		return *str != '\0' ? CTLengthRecursive(str + 1, position + 1) : position;
	}

	static constexpr int CTFindCharRecursive(const char* str, const char ch, const int position) {

		if (*str == '\0') {
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

	static constexpr int CTCountCharRecursive(const char* str, const char ch, const int position, int count) {

		if (*str == '\0') {
			return count;
		}

		if (*str == ch) {
			count++;
		}

		return CTCountCharRecursive(str + 1, ch, position + 1, count);
	}
};

NS_JC_END

