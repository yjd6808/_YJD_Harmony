/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <type_traits>
#include <cerrno>

#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Assert.h"
#include "jc/Ascii.h"
#include "jc/Namespace.h"

NS_JC_BEGIN
template <typename CharT> class StringUtil;
NS_END

#include "jc/Primitives/String.h"
#include "jc/Container/Vector.h"
#include "jc/Tuple.h"
#include "jc/Wrapper/CRuntime.h"

NS_JC_BEGIN

class CDefaultAllocator;

template <typename CharT>
class StringUtil final
{
public:
	using StrType = BasicString<CharT, StringImpl_SSO<CharT>>;

	// buf에 str 문자열 추가함
	static void ConcatInnerBack(CharT* _pBuf, int _buflen, int _bufCapacity, const CharT* _pConcatStr, int _concatStrLen);
	static void ConcatInnerFront(CharT* _pBuf, int _buflen, int _bufCapacity, const CharT* _pConcatStr, int _concatStrLen);
	static void ConcatInnerFront(CharT* _pBuf, int _bufCapacity, const CharT* _pConcatStr);

	static int Copy(CharT* _pBuffer, int _bufferSize, const CharT* _pCopy);
	static int CopyUnsafe(CharT* _pBuffer, const CharT* _pCopy);

	// 컴파일 타임용
	template <typename T, typename U>
	static constexpr int CTCompare(T&& _src, U&& _dst) {
		if (_src == nullptr && _dst == nullptr)
			return 0;
		if (_src == nullptr)
			return -1;
		if (_dst == nullptr)
			return 1;

		const int ISRC_LEN = CTLength(_src);
		const int IDST_LEN = CTLength(_dst);

		CharT* pSrc = (CharT*)_src;
		CharT* pDst = (CharT*)_dst;

		while (*pDst != CharT(0) && *pSrc != CharT(0)) {
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

	static constexpr void CTCopy(CharT* _pBuffer, int _bufferSize, const CharT* _pCopy, int _count)
	{
		if (_pBuffer == nullptr || _pCopy == nullptr || _bufferSize <= 0)
		{
			return;
		}
		int iSize = 0;
		while (*_pCopy != CharT(0) && iSize < _bufferSize - 1 && iSize < _count)
		{
			*_pBuffer = *_pCopy;
			_pBuffer++;
			_pCopy++;
			iSize++;
		}
		*_pBuffer = CharT(0);
	}

	static constexpr int CTCount(const CharT* _pStr, const CharT* _pTarget) {
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
	static constexpr int CTCountChar(const CharT* _pStr, const CharT _ch) {
		return CTCountCharRecursive(_pStr, _ch, 0, 0);
	}

	static constexpr int CTFind(const CharT* _pSource, const CharT* _pTarget, bool _caseSensitive = true)
	{
		return CTFind(_pSource, _pTarget, 0, _caseSensitive);
	}

	static constexpr int CTFind(const CharT* _pSource, const CharT* _pTarget, int _startOffset, bool _caseSensitive = true)
	{
		const int SOURCE_LEN = CTLength(_pSource);
		const int TARGET_LEN = CTLength(_pTarget);
		return CTFind(_pSource, SOURCE_LEN, _pTarget, TARGET_LEN, _startOffset, _caseSensitive);
	}

	static constexpr int CTFind(
		const CharT* _pSource,
		int _sourceLen,
		const CharT* _pTarget,
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
	static constexpr int CTFindChar(const CharT* _pStr, const CharT _ch)
	{
		return CTFindCharRecursive(_pStr, _ch, 0);
	}

	static constexpr int CTFindChar(const CharT* _pStr, const CharT _ch, int _startOffset)
	{
		return CTFindCharRecursive(_pStr + _startOffset, _ch, _startOffset);
	}

	// 문자열에서 문자를 찾아서 인덱스값을 반환 (뒤에서부터)
	static constexpr int CTFindCharReverse(const CharT* _pStr, const CharT _ch) {
		const int STR_LEN = CTLength(_pStr);
		return CTFindCharReverseRecursive(_pStr + STR_LEN - 1, _ch, STR_LEN);
	}

	static constexpr int CTLength(const CharT* _pStr)
	{
		if (_pStr == nullptr)
		{
			return 0;
		}
		int iLength = 0;
		while (*_pStr != CharT(0))
		{
			iLength++;
			_pStr++;
		}
		return iLength;
	}

	template <_u32 CAP>
	static constexpr int CTLength(const CharT(&_str)[CAP])
	{
		return CAP - 1;
	}

	static constexpr int CTToInt32(const CharT* _pStr)
	{
		return CTToNumber<_s32>(_pStr);
	}

	static constexpr _s64 CTToInt64(const CharT* _pStr)
	{
		return CTToNumber<_s64>(_pStr);
	}

	static constexpr void CTTrim(CharT* _pBuffer, int _bufferSize, CharT _ch = CharT(' '))
	{
		CTTrimLeft(_pBuffer, _bufferSize, _ch);
		CTTrimRight(_pBuffer, _bufferSize, _ch);
	}

	static constexpr void CTTrimLeft(CharT* _pBuffer, int _bufferSize, CharT _ch = CharT(' '))
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

	static constexpr void CTTrimRight(CharT* _pBuffer, int _bufferSize, CharT _ch = CharT(' '))
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

	static constexpr void CTZeroMemory(CharT* _pBuffer, int _bufferSize)
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
	static StrType FillLeft(const T& _v, CharT _paddingCharacter, int _len);

	static int Find(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true);
	static int Find(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, int _strLen, bool _caseSensitive = true);
	static int Find(const CharT* _pSource, int _sourceLen, int _startIdx, const CharT* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const CharT* _pSource, const CharT* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const CharT* _pSource, int _sourceLen, const CharT* _pStr, bool _caseSensitive = true);
	static int FindAll(OUT int* _pPositionArray, const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true);
	static int FindChar(const CharT* _pSource, CharT _ch);
	static int FindCharReverse(const CharT* _pSource, CharT _ch);
	static int FindCharReverse(const CharT* _pSource, int _len, CharT _ch);
	static int FindCharUncontained(const CharT* _pSource, CharT _ch);

	static StrType Format(const CharT* _pFormat, ...);
	static StrType Format(const CharT* _pFormat, va_list _args);
	static void FormatBuffer(CharT* _pBuff, int _buffCapacity, const CharT* _pFormat, ...);
	static void FormatBuffer(CharT* _pBuff, int _buffCapacity, const CharT* _pFormat, va_list _args);

	static StrType GetRange(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx);
	static Tuple<CharT*, int, int> GetRangeUnsafe(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx);
	static StrType SubStr(const CharT* _pSource, int _sourceLen, int _startIdx, int _count);

	static bool IsEqual(const CharT* _pSrc, const CharT* _pDst, bool _bCompareCase = true);
	static bool IsEqual(const CharT* _pSrc, int _srcLen, const CharT* _pDst, int _dstLen, bool _bCompareCase = true);
	static bool IsNullOrEmpty(const CharT* _pStr);

	static int Compare(const CharT* _pStr, const CharT* _pStr2);
	static int Compare(const CharT* _pSrc, int _srcLen, const CharT* _pDst, int _dstLen);

	constexpr static int Length(const CharT* _pStr) {
		if (_pStr == nullptr) {
			return -1;
		}

		int iSize = 0;
		while (*_pStr != CharT(0)) {
			iSize++;
			_pStr++;
		}
		return iSize;
	}

	constexpr static int LengthWithNull(const CharT* _pStr) {
		return Length(_pStr) + 1;
	}

	static const CharT* SkipLeadingChar(const CharT* _pStr, CharT _skipChar);
	static const CharT* SkipLeadingNumberZero(const CharT* _pStr);

	static Vector<StrType, CDefaultAllocator> Split(StrType& _src, const CharT* _pDelimiter);
	static Vector<StrType, CDefaultAllocator> Split(StrType& _src, CharT _delimiter);

	static void Swap(StrType& _src, StrType& _dst);

	template <typename TInteger>
	static TInteger ToNumber(const CharT* _pStr, OUT CharT** _ppEndptr = nullptr, bool _ignoreLeadingZero = true);

	static int ToStringBuffered(CharT* _pBuff, int _capacity, bool _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _s8 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _u8 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _s16 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _u16 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _s32 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _u32 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _s32l _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _u32l _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _s64 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, _u64 _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, float _value);
	static int ToStringBuffered(CharT* _pBuff, int _capacity, double _value);

	static StrType ToString(bool _value);
	static StrType ToString(_s8 _value);
	static StrType ToString(_u8 _value);
	static StrType ToString(_s16 _value);
	static StrType ToString(_u16 _value);
	static StrType ToString(_s32 _value);
	static StrType ToString(_u32 _value);
	static StrType ToString(_s32l _value);
	static StrType ToString(_u32l _value);
	static StrType ToString(_s64 _value);
	static StrType ToString(_u64 _value);
	static StrType ToString(float _value);
	static StrType ToString(double _value);
	static StrType ToString(const StrType& _value);
	static StrType ToString(StrType&& _value);

	// https://stackoverflow.com/questions/26080829/detecting-strtol-failure
	template <typename TInteger>
	static bool TryToNumber(OUT TInteger& _val, const CharT* _pStr, bool _ignoreLeadingZero = true) {
		errno = 0;
		CharT* pEnd = nullptr;
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
	static constexpr T CTToNumber(const CharT* _pStr)
	{
		if (_pStr == nullptr)
		{
			return 0;
		}
		T result = 0;
		int sign = 1;
		const CharT* p = _pStr;
		if (*p == CharT('-')) {
			sign = -1;
			p++;
		}

		while (*p >= CharT('0') && *p <= CharT('9'))
		{
			result = result * 10 + (*p - CharT('0'));
			p++;
		}
		return sign * result;
	}

	static constexpr int CTLengthRecursive(const CharT* _pStr, const int _position) {
		return *_pStr != CharT(0) ? CTLengthRecursive(_pStr + 1, _position + 1) : _position;
	}

	static constexpr int CTFindCharRecursive(const CharT* _pStr, const CharT _ch, const int _position) {
		if (*_pStr == CharT(0)) {
			return -1;
		}

		if (*_pStr == _ch) {
			return _position;
		}

		return CTFindCharRecursive(_pStr + 1, _ch, _position + 1);
	}

	static constexpr int CTFindCharReverseRecursive(const CharT* _pStr, const CharT _ch, const int _position) {
		if (_position == 0) {
			return -1;
		}

		if (*_pStr == _ch) {
			return _position - 1;
		}

		return CTFindCharReverseRecursive(_pStr - 1, _ch, _position - 1);
	}

	static constexpr int CTCountCharRecursive(const CharT* _pStr, const CharT _ch, const int _position, int _count) {
		if (*_pStr == CharT(0)) {
			return _count;
		}

		if (*_pStr == _ch) {
			_count++;
		}

		return CTCountCharRecursive(_pStr + 1, _ch, _position + 1, _count);
	}
};

using StringUtilA = StringUtil<char>;
using StringUtilW = StringUtil<wchar_t>;
using StringUtilT = StringUtil<_char>;

NS_END

#define JC_FMT(...) jc::StringUtilT::Format(__VA_ARGS__)

#include "jc/Primitives/StringUtil.inl"
