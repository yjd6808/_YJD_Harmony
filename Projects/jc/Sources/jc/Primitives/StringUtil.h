/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#pragma once

#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Assert.h"
#include "jc/Ascii.h"
#include "jc/Namespace.h"

NS_JC_BEGIN

class CDefaultAllocator;

template <typename...>
struct Tuple;

template <typename, typename>
class Vector;
class String;
class StringUtil final
{
public:
	// buf에 str 문자열 추가함
	static void ConcatInnerBack(char* _pBuf, int _buflen, int _bufCapacity, const char* _pConcatStr, int _concatStrLen);
	static void ConcatInnerFront(char* _pBuf, int _buflen, int _bufCapacity, const char* _pConcatStr, int _concatStrLen);
	static void ConcatInnerFront(char* _pBuf, int _bufCapacity, const char* _pConcatStr);

	static int Copy(char* _pBuffer, int _bufferSize, const char* _pCopy);
	static int CopyUnsafe(char* _pBuffer, const char* _pCopy);

	// 컴파일 타임용
	template <typename T, typename U>
	static constexpr int CTCompare(T&& _src, U&& _dst) {
		const int ISRC_LEN = CTLength(_src);
		const int IDST_LEN = CTLength(_dst);

		char* pSrc = (char*)_src;
		char* pDst = (char*)_dst;

		while (*pDst != NULL && *pSrc != NULL) {
			if (*pDst > *pSrc)
				return -1;
			else if (*pDst < *pSrc)
				return 1;

			++pDst;
			++pSrc;
		}

		if (IDST_LEN > ISRC_LEN)
			return -1;
		else if (IDST_LEN < ISRC_LEN)
			return 1;

		return 0;
	}

	static constexpr void CTCopy(char* _pBuffer, int _bufferSize, const char* _pCopy, int _count)
	{
		if (_pBuffer == nullptr || _pCopy == nullptr || _bufferSize <= 0)
		{
			return;
		}
		int iSize = 0;
		while (*_pCopy != NULL && iSize < _bufferSize - 1 && iSize < _count)
		{
			*_pBuffer = *_pCopy;
			_pBuffer++;
			_pCopy++;
			iSize++;
		}
		*_pBuffer = NULL;
	}

	static constexpr int CTCount(const char* _pStr, const char* _pTarget) {
		const int STR_LEN = CTLength(_pStr);
		const int TARGET_LEN = CTLength(_pTarget);
		if (TARGET_LEN == 0 || TARGET_LEN > STR_LEN)
			return 0;
		int count = 0;
		for (int i = 0; i <= STR_LEN - TARGET_LEN; ++i) {
			bool match = true;
			for (int j = 0; j < TARGET_LEN; ++j) {
				if (_pStr[i + j] != _pTarget[j]) {
					match = false;
					break;
				}
			}
			if (match)
				count++;
		}
		return count;
	}

	// 문자열에서 특정 문자의 개수를 반환
	static constexpr int CTCountChar(const char* _pStr, const char _ch) {
		return CTCountCharRecursive(_pStr, _ch, 0, 0);
	}

	static constexpr int CTFind(const char* _pSource, const char* _pTarget, bool _caseSensitive = true)
	{
		return CTFind(_pSource, _pTarget, 0, _caseSensitive);
	}

	static constexpr int CTFind(const char* _pSource, const char* _pTarget, int _startOffset, bool _caseSensitive = true)
	{
		const int SOURCE_LEN = CTLength(_pSource);
		const int TARGET_LEN = CTLength(_pTarget);
		return CTFind(_pSource, SOURCE_LEN, _pTarget, TARGET_LEN, _startOffset, _caseSensitive);
	}

	static constexpr int CTFind(
		const char* _pSource,
		int _sourceLen,
		const char* _pTarget,
		int _targetLen,
		int _startOffset,
		bool _caseSensitive = true)
	{
		if (_targetLen == 0 || _targetLen > _sourceLen)
			return -1;

		for (int i = _startOffset; i <= _sourceLen - _targetLen; ++i) {
			bool match = true;
			for (int j = 0; j < _targetLen; ++j) {
				if (_caseSensitive ? _pSource[i + j] != _pTarget[j] : jc::ToLower(_pSource[i + j]) != jc::ToLower(_pTarget[j])) {
					match = false;
					break;
				}
			}
			if (match)
				return i;
		}
		return -1;
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 (앞에서부터)
	static constexpr int CTFindChar(const char* _pStr, const char _ch)
	{
		return CTFindCharRecursive(_pStr, _ch, 0);
	}

	static constexpr int CTFindChar(const char* _pStr, const char _ch, int _startOffset)
	{
		return CTFindCharRecursive(_pStr + _startOffset, _ch, _startOffset);
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 (뒤에서부터)
	static constexpr int CTFindCharReverse(const char* _pStr, const char _ch) {
		const int STR_LEN = CTLength(_pStr);
		return CTFindCharReverseRecursive(_pStr + STR_LEN - 1, _ch, STR_LEN);
	}

	static constexpr int CTLength(const char* _pStr)
	{
		int iLength = 0;
		while (*_pStr != '\0')
		{
			iLength++;
			_pStr++;
		}
		return iLength;
	}

	template <_u32 CAP>
	static constexpr int CTLength(const char(&_str)[CAP])
	{
		return CAP - 1;
	}

	static constexpr int CTToInt32(const char* _pStr)
	{
		return CTToNumber<_s32>(_pStr);
	}

	static constexpr _s64 CTToInt64(const char* _pStr)
	{
		return CTToNumber<_s64>(_pStr);
	}

	static constexpr void CTTrim(char* _pBuffer, int _bufferSize, char _ch = ' ')
	{
		CTTrimLeft(_pBuffer, _bufferSize, _ch);
		CTTrimRight(_pBuffer, _bufferSize, _ch);
	}

	static constexpr void CTTrimLeft(char* _pBuffer, int _bufferSize, char _ch = ' ')
	{
		if (_pBuffer == nullptr || _bufferSize <= 0)
		{
			return;
		}
		int i = 0;
		while (i < _bufferSize && _pBuffer[i] == _ch)
		{
			i++;
		}
		if (i > 0)
		{
			for (int j = 0; j < _bufferSize - i; ++j)
			{
				_pBuffer[j] = _pBuffer[j + i];
			}
			for (int j = _bufferSize - i; j < _bufferSize; ++j)
			{
				_pBuffer[j] = 0;
			}
		}
	}

	static constexpr void CTTrimRight(char* _pBuffer, int _bufferSize, char _ch = ' ')
	{
		if (_pBuffer == nullptr || _bufferSize <= 0)
		{
			return;
		}
		int i = _bufferSize - 1;
		while (i >= 0 && _pBuffer[i] == _ch)
		{
			i--;
		}
		if (i < _bufferSize - 1)
		{
			for (int j = i + 1; j < _bufferSize; ++j)
			{
				_pBuffer[j] = 0;
			}
		}
	}

	static constexpr void CTZeroMemory(char* _pBuffer, int _bufferSize)
	{
		if (_pBuffer == nullptr || _bufferSize <= 0)
		{
			return;
		}
		for (int i = 0; i < _bufferSize; ++i)
		{
			_pBuffer[i] = 0;
		}
	}

	// FillLeft(20, '0', 3) -> 020
	// FillLeft(20, '0', 4) -> 0020
	template <typename T>
	static String FillLeft(const T& _v, char _paddingCharacter, int _len) {
		if (_len >= 1023)
		{
			jc_assert(false);
			return jc::String(0);
		}

		char szFill[1024];
		String szRet;
		szRet += _v;

		const int iFillCount = _len - szRet.Length();
		int i = 0;
		for (i = 0; i < iFillCount; ++i) {
			szFill[i] = _paddingCharacter;
		}
		szFill[i] = '\0';
		szRet.Insert(0, szFill);
		return szRet;
	}

	static int Find(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true);
	static int Find(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr, int _strLen, bool _caseSensitive = true);
	static int Find(const char* _pSource, int _sourceLen, int _startIdx, const char* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const char* _pSource, const char* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const char* _pSource, int _sourceLen, const char* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true);
	static int FindChar(const char* _pSource, char _ch);
	static int FindCharReverse(const char* _pSource, char _ch);
	static int FindCharReverse(const char* _pSource, int _len, char _ch);
	static int FindCharUncontained(const char* _pSource, char _ch);

	static String Format(const char* _pFormat, ...);
	static String Format(const char* _pFormat, va_list _args);
	static void FormatBuffer(char* _pBuff, int _buffCapacity, const char* _pFormat, ...);
	static void FormatBuffer(char* _pBuff, int _buffCapacity, const char* _pFormat, va_list _args);

	static String GetRange(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx);
	static Tuple<char*, int, int> GetRangeUnsafe(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx);
	static String SubStr(const char* _pSource, int _sourceLen, int _startIdx, int _count);

	static bool IsEqual(const char* _pSrc, const char* _pDst, bool _bCompareCase = true);
	static bool IsEqual(const char* _pSrc, int _srcLen, const char* _pDst, int _dstLen, bool _bCompareCase = true);
	static bool IsNullOrEmpty(const char* _pStr);

	constexpr static int Length(const char* _pStr) {
		if (_pStr == nullptr) {
			return -1;
		}

		int iSize = 0;
		while (*_pStr != NULL) {
			iSize++;
			_pStr++;
		}
		return iSize;
	}

	constexpr static int LengthWithNull(const char* _pStr) {
		return Length(_pStr) + 1;
	}

	static const char* SkipLeadingChar(const char* _pStr, char _skipChar);
	static const char* SkipLeadingNumberZero(const char* _pStr);

	static Vector<String, CDefaultAllocator> Split(String& _src, const char* _pDelimiter);
	static Vector<String, CDefaultAllocator> Split(String& _src, char _delimiter);

	static void Swap(String& _src, String& _dst);

	template <typename TInteger>
	static TInteger ToNumber(const char* _pStr, OUT char** _ppEndptr = nullptr, bool _ignoreLeadingZero = true);

	template <typename TNumber>
	static int ToStringBuffered(char* _pBuff, int _capacity, TNumber _value);

	template <typename TNumber>
	static String ToString(TNumber _value);

	static jc::String ToUtf8(const wchar_t* _pStr, int _length);

	// https://stackoverflow.com/questions/26080829/detecting-strtol-failure
	template <typename TInteger>
	static bool TryToNumber(OUT TInteger& _val, const char* _pStr, bool _ignoreLeadingZero = true) {
		errno = 0;
		char* pEnd = nullptr;
		TInteger v = ToNumber<TInteger>(_pStr, &pEnd, _ignoreLeadingZero);

		if (pEnd == _pStr) {	// 숫자 못찾는 경우 에로노 셋안됨
			return false;
		}

		if (errno != 0) {
			return false;
		}
		_val = v;
		return true;
	}

private:
	template <typename T>
	static constexpr T CTToNumber(const char* _pStr)
	{
		T result = 0;
		int sign = 1;
		const char* p = _pStr;
		if (*p == '-') {
			sign = -1;
			p++;
		}

		while (*p >= '0' && *p <= '9')
		{
			result = result * 10 + (*p - '0');
			p++;
		}
		return sign * result;
	}

	static constexpr int CTLengthRecursive(const char* _pStr, const int _position) {
		return *_pStr != '\0' ? CTLengthRecursive(_pStr + 1, _position + 1) : _position;
	}

	static constexpr int CTFindCharRecursive(const char* _pStr, const char _ch, const int _position) {
		if (*_pStr == '\0') {
			return -1;
		}

		if (*_pStr == _ch) {
			return _position;
		}

		return CTFindCharRecursive(_pStr + 1, _ch, _position + 1);
	}

	static constexpr int CTFindCharReverseRecursive(const char* _pStr, const char _ch, const int _position) {
		if (_position == 0) {
			return -1;
		}

		if (*_pStr == _ch) {
			return _position - 1;
		}

		return CTFindCharReverseRecursive(_pStr - 1, _ch, _position - 1);
	}

	static constexpr int CTCountCharRecursive(const char* _pStr, const char _ch, const int _position, int _count) {
		if (*_pStr == '\0') {
			return _count;
		}

		if (*_pStr == _ch) {
			_count++;
		}

		return CTCountCharRecursive(_pStr + 1, _ch, _position + 1, _count);
	}
};

NS_END

#define JC_FMT(...) jc::StringUtil::Format(__VA_ARGS__)
