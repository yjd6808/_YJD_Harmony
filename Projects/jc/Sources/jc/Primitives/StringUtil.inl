/*
	작성자 : 윤정도
	StringUtil<CharT> 본문 정의. StringUtil.h 끝에서 include된다 (별칭 이후).
	직접 include하지 않는다.
*/

#pragma once

#include <cstdlib>
#include <cstdarg>
#include <utility>

#include "jc/Primitives/StringUtil.h"

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::ConcatInnerBack(CharT* _pBuf, int _buflen, int _bufCapacity, const CharT* _pConcatStr, int _concatStrLen)
{
	jc_assert_msg(_buflen + _concatStrLen + 1 <= _bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafe(_pBuf + _buflen, _pConcatStr, _concatStrLen * static_cast<int>(sizeof(CharT)));
	_pBuf[_buflen + _concatStrLen] = CharT(0);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::ConcatInnerFront(CharT* _pBuf, int _buflen, int _bufCapacity, const CharT* _pConcatStr, int _concatStrLen)
{
	jc_assert_msg(_buflen + _concatStrLen + 1 <= _bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafeReverse(_pBuf + _concatStrLen, _pBuf, _buflen * static_cast<int>(sizeof(CharT)));
	Memory::CopyUnsafe(_pBuf, _pConcatStr, _concatStrLen * static_cast<int>(sizeof(CharT)));
	_pBuf[_buflen + _concatStrLen] = CharT(0);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::ConcatInnerFront(CharT* _pBuf, int _bufCapacity, const CharT* _pConcatStr)
{
	int iBufLen = Length(_pBuf);
	int iConcatLen = Length(_pConcatStr);
	ConcatInnerFront(_pBuf, iBufLen, _bufCapacity, _pConcatStr, iConcatLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Copy(CharT* _pBuffer, const int _bufferSize, const CharT* _pCopy)
{
	if (_pBuffer == nullptr || _pCopy == nullptr)
	{
		return -1;
	}

	int iSize = 0;
	CharT* pBuffer = _pBuffer;

	while (*_pCopy != CharT(0) && iSize < _bufferSize)
	{
		*_pBuffer = *_pCopy;
		_pBuffer++;
		_pCopy++;
		iSize++;
	}

	pBuffer[iSize] = CharT(0);
	return iSize;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::CopyUnsafe(CharT* _pBuffer, const CharT* _pCopy)
{
	if (_pBuffer == nullptr || _pCopy == nullptr)
	{
		return -1;
	}

	int iSize = 0;
	CharT* pBuffer = _pBuffer;

	while (*_pCopy != CharT(0))
	{
		*_pBuffer = *_pCopy;
		_pBuffer++;
		_pCopy++;
		iSize++;
	}

	pBuffer[iSize] = CharT(0);
	return iSize;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Find(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive /*= true*/)
{
	return Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, Length(_pStr), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Find(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, int _strLen, bool _caseSensitive /*= true*/)
{
	const int iFindStrLen = _strLen;
	const int iSearchLen = _endIdx - _startIdx + 1;

	if (_strLen == 0)
	{
		return 0;
	}

	if (_startIdx > _endIdx || _startIdx < 0 || _endIdx >= _sourceLen)
	{
		return -1;
	}

	if (iFindStrLen > iSearchLen)
	{
		return -1;
	}

	for (int i = _startIdx; i <= _endIdx; i++)
	{
		int iContinuous = 0;

		if (_caseSensitive)
		{
			while (iContinuous < iFindStrLen && _pSource[i + iContinuous] == _pStr[iContinuous])
			{
				iContinuous++;
			}
		}
		else
		{
			while (iContinuous < iFindStrLen && jc::ToLower(_pSource[i + iContinuous]) == jc::ToLower(_pStr[iContinuous]))
			{
				iContinuous++;
			}
		}

		if (iContinuous == iFindStrLen)
		{
			return i;
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Find(const CharT* _pSource, int _sourceLen, int _startIdx, const CharT* _pStr, bool _caseSensitive /*= true*/)
{
	return Find(_pSource, _sourceLen, _startIdx, _sourceLen - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindAll(OUT int* _pPositionArray, const CharT* _pSource, const CharT* _pStr, bool _caseSensitive /*= true*/)
{
	const int iSourceLength = Length(_pSource);
	return FindAll(_pPositionArray, _pSource, iSourceLength, 0, iSourceLength - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindAll(OUT int* _pPositionArray, const CharT* _pSource, int _sourceLen, const CharT* _pStr, bool _caseSensitive /*= true*/)
{
	return FindAll(_pPositionArray, _pSource, _sourceLen, 0, _sourceLen - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindAll(OUT int* _pPositionArray, const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive /*= true*/)
{
	if (_endIdx < _startIdx)
	{
		return 0;
	}

	int iCount = 0;
	int iFindPos = -1;
	int iStrLength = Length(_pStr);

	while ((iFindPos = Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, iStrLength, _caseSensitive)) != -1)
	{
		_pPositionArray[iCount++] = iFindPos;
		_startIdx = iFindPos + iStrLength;
	}

	return iCount;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindChar(const CharT* _pSource, CharT _ch)
{
	int i = 0;

	while (true)
	{
		const CharT sch = *(_pSource + i);

		if (sch == CharT(0))
		{
			return -1;
		}

		if (sch == _ch)
		{
			return i;
		}

		++i;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindCharReverse(const CharT* _pSource, CharT _ch)
{
	return FindCharReverse(_pSource, Length(_pSource), _ch);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindCharReverse(const CharT* _pSource, int _len, CharT _ch)
{
	while ((--_len) >= 0)
	{
		if (_pSource[_len] == _ch)
			return _len;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::FindCharUncontained(const CharT* _pSource, CharT _ch)
{
	int i = 0;

	while (true)
	{
		const CharT sch = *(_pSource + i);

		if (sch == CharT(0))
		{
			return i;
		}

		if (sch != _ch)
		{
			return i;
		}

		++i;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::Format(const CharT* _pFormat, ...)
{
	va_list args;
	va_start(args, _pFormat);
	StrType szResult = Format(_pFormat, args);
	va_end(args);
	return szResult;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::Format(const CharT* _pFormat, va_list _args)
{
	va_list argsCopy;
	va_copy(argsCopy, _args);
	const int iExpectedLen = CRuntime::FormatLengthV(_pFormat, argsCopy);
	va_end(argsCopy);

	if (iExpectedLen <= 0)
	{
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	const int iCapacity = iExpectedLen + 1 + StrType::DEFAULT_BUFFER_SIZE;
	CharT* pBuffer = Memory::Allocate<CharT*>(static_cast<_u32>(iCapacity * sizeof(CharT)));
	CRuntime::FormatV(pBuffer, iCapacity, _pFormat, _args);
	pBuffer[iExpectedLen] = CharT(0);

	StrType szResult;
	szResult.Adopt(pBuffer, iExpectedLen, iCapacity);
	return szResult;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::FormatBuffer(CharT* _pBuff, const int _buffCapacity, const CharT* _pFormat, ...)
{
	va_list args;
	va_start(args, _pFormat);
	FormatBuffer(_pBuff, _buffCapacity, _pFormat, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::FormatBuffer(CharT* _pBuff, const int _buffCapacity, const CharT* _pFormat, va_list _args)
{
	va_list argsCopy;
	va_copy(argsCopy, _args);
	const int iExpectedLen = CRuntime::FormatLengthV(_pFormat, argsCopy);
	va_end(argsCopy);
	jc_assert_msg(iExpectedLen > 0, "문자열 포맷 수행중 오류가 발생하였습니다.");
	jc_assert_msg(iExpectedLen < _buffCapacity, "문자열 포맷 수행중 오류가 발생하였습니다. (문자열 길이가 버퍼의 용량을 초과합니다.)");
	CRuntime::FormatV(_pBuff, _buffCapacity, _pFormat, _args);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::GetRange(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx)
{
	auto [pBuffer, iLen, iCapacity] = GetRangeUnsafe(_pSource, _sourceLen, _startIdx, _endIdx);

	StrType subString;
	subString.Adopt(pBuffer, iLen, iCapacity);
	return subString;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
Tuple<CharT*, int, int> StringUtil<CharT>::GetRangeUnsafe(const CharT* _pSource, int _sourceLen, int _startIdx, int _endIdx)
{
	if (_startIdx > _endIdx || _startIdx < 0 || _endIdx >= _sourceLen)
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	const CharT* pStr = _pSource + _startIdx;
	(void)pStr;
	int iCurIdx = _startIdx;
	int iIdx = 0;
	const int iAllocCapacity = _endIdx - _startIdx + 10;
	CharT* szRange = Memory::Allocate<CharT*>(static_cast<_u32>(iAllocCapacity * sizeof(CharT)));

	while (iCurIdx <= _endIdx)
	{
		szRange[iIdx] = _pSource[iCurIdx];
		iCurIdx++;
		iIdx++;
	}

	szRange[iIdx] = CharT(0);
	return { szRange, iIdx, iAllocCapacity };
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::SubStr(const CharT* _pSource, int _sourceLen, int _startIdx, int _count)
{
	if (_startIdx < 0 || _startIdx >= _sourceLen || _count < 0)
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	int endIdx = _startIdx + _count - 1;
	if (endIdx >= _sourceLen)
	{
		endIdx = _sourceLen - 1;
	}
	return GetRange(_pSource, _sourceLen, _startIdx, endIdx);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool StringUtil<CharT>::IsEqual(const CharT* _pSrc, const CharT* _pDst, bool _bCompareCase/*= true*/)
{
	return IsEqual(_pSrc, Length(_pSrc), _pDst, Length(_pDst), _bCompareCase);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool StringUtil<CharT>::IsEqual(const CharT* _pSrc, const int _srcLen, const CharT* _pDst, const int _dstLen, bool _bCompareCase /*= true*/)
{
	for (int i = 0, j = 0; i < _srcLen && j < _dstLen; i++, j++)
	{
		if (_bCompareCase == false)
		{
			if (jc::ToLower(*(_pSrc + i)) != jc::ToLower(*(_pDst + j)))
			{
				return false;
			}
			continue;
		}

		if (*(_pSrc + i) != *(_pDst + j))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool StringUtil<CharT>::IsNullOrEmpty(const CharT* _pStr)
{
	if (_pStr == nullptr)
		return true;
	if (_pStr[0] == CharT(0))
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Compare(const CharT* _pStr1, const CharT* _pStr2)
{
	return Compare(_pStr1, Length(_pStr1), _pStr2, Length(_pStr2));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::Compare(const CharT* _pStr1, int _str1Len, const CharT* _pStr2, int _str2Len)
{
	const CharT* pSrc = _pStr1;
	const CharT* pDst = _pStr2;

	while (*pSrc != CharT(0) && *pDst != CharT(0))
	{
		if (*pSrc > *pDst)
			return 1;
		if (*pSrc < *pDst)
			return -1;

		pSrc++;
		pDst++;
	}

	if (_str1Len > _str2Len)
		return 1;
	if (_str1Len < _str2Len)
		return -1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
const CharT* StringUtil<CharT>::SkipLeadingChar(const CharT* _pStr, CharT _skipChar)
{
	const int iPos = FindCharUncontained(_pStr, _skipChar);
	return _pStr + iPos;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
const CharT* StringUtil<CharT>::SkipLeadingNumberZero(const CharT* _pStr)
{
	const int iStrLen = Length(_pStr);

	if (iStrLen == 0)
	{
		return _pStr;
	}

	const CharT* szSkippedLeadingZero = SkipLeadingChar(_pStr, CharT('0'));
	if (szSkippedLeadingZero == _pStr + iStrLen)
	{
		if constexpr (std::is_same_v<CharT, char>)
			return "0";
		else
			return L"0";
	}

	return szSkippedLeadingZero;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
Vector<typename StringUtil<CharT>::StrType, CDefaultAllocator> StringUtil<CharT>::Split(StrType& _src, const CharT* _pDelimiter)
{
	return _src.Split(_pDelimiter);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
Vector<typename StringUtil<CharT>::StrType, CDefaultAllocator> StringUtil<CharT>::Split(StrType& _src, CharT _delimiter)
{
	return _src.Split(_delimiter);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
void StringUtil<CharT>::Swap(StrType& _src, StrType& _dst)
{
	StrType temp = std::move(_src);
	_src = std::move(_dst);
	_dst = std::move(temp);
}

// ========================================================================================
// ToNumber 단일 본문
// ========================================================================================

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
template <typename TInteger>
TInteger StringUtil<CharT>::ToNumber(const CharT* _pStr, OUT CharT** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */)
{
	const CharT* pStr = _ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr;

	if constexpr (std::is_same_v<TInteger, _s8>)
		return static_cast<_s8>(CRuntime::StrToLong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _u8>)
		return static_cast<_u8>(CRuntime::StrToULong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _s16>)
		return static_cast<_s16>(CRuntime::StrToLong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _s16c>)
		return static_cast<_s16c>(CRuntime::StrToULong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _u16>)
		return static_cast<_u16>(CRuntime::StrToULong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _s32>)
		return static_cast<_s32>(CRuntime::StrToLong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _u32>)
		return static_cast<_u32>(CRuntime::StrToULong(pStr, _ppEndptr, 10));
	else if constexpr (std::is_same_v<TInteger, _s32l>)
		return CRuntime::StrToLong(pStr, _ppEndptr, 10);
	else if constexpr (std::is_same_v<TInteger, _u32l>)
		return CRuntime::StrToULong(pStr, _ppEndptr, 10);
	else if constexpr (std::is_same_v<TInteger, _s64>)
		return CRuntime::StrToLLong(pStr, _ppEndptr, 10);
	else if constexpr (std::is_same_v<TInteger, _u64>)
		return CRuntime::StrToULLong(pStr, _ppEndptr, 10);
	else if constexpr (std::is_same_v<TInteger, float>)
		return CRuntime::StrToFloat(pStr, _ppEndptr);
	else if constexpr (std::is_same_v<TInteger, double>)
		return CRuntime::StrToDouble(pStr, _ppEndptr);
	else
		static_assert(sizeof(TInteger) == 0, "지원하지 않는 수치 타입입니다.");
}

// ========================================================================================
// ToStringBuffered 오버로드
// ========================================================================================

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, bool _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%s", _value ? "true" : "false");
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%s", _value ? L"true" : L"false");
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _s8 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%d", static_cast<_s32>(_value));
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%d", static_cast<_s32>(_value));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _u8 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%u", static_cast<_u32>(_value));
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%u", static_cast<_u32>(_value));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _s16 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%d", static_cast<_s32>(_value));
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%d", static_cast<_s32>(_value));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _u16 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%u", static_cast<_u32>(_value));
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%u", static_cast<_u32>(_value));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _s32 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%d", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%d", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _u32 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%u", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%u", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _s32l _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%ld", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%ld", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _u32l _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%lu", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%lu", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _s64 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%lld", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%lld", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, _u64 _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%llu", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%llu", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, float _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%g", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%g", _value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
int StringUtil<CharT>::ToStringBuffered(CharT* _pBuff, int _capacity, double _value)
{
	if constexpr (std::is_same_v<CharT, char>)
		return CRuntime::FormatBuffered(_pBuff, _capacity, "%g", _value);
	else
		return CRuntime::FormatBuffered(_pBuff, _capacity, L"%g", _value);
}

// ========================================================================================
// ToString 오버로드
// ========================================================================================

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(bool _value)
{
	CharT buf[8];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_s8 _value)
{
	CharT buf[8];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_u8 _value)
{
	CharT buf[8];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_s16 _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_u16 _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_s32 _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_u32 _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_s32l _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_u32l _value)
{
	CharT buf[16];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_s64 _value)
{
	CharT buf[32];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(_u64 _value)
{
	CharT buf[32];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(float _value)
{
	CharT buf[64];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(double _value)
{
	CharT buf[64];
	ToStringBuffered(buf, _countof(buf), _value);
	return buf;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(const StrType& _value)
{
	return _value;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringUtil<CharT>::StrType StringUtil<CharT>::ToString(StrType&& _value)
{
	return std::move(_value);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
template <typename T>
typename StringUtil<CharT>::StrType StringUtil<CharT>::FillLeft(const T& _v, CharT _paddingCharacter, int _len) {
	if (_len >= 1023)
	{
		jc_assert(false);
		return StrType(0);
	}

	CharT szFill[1024];
	StrType szRet;
	szRet += _v;

	const int iFillCount = _len - szRet.Length();
	int i = 0;
	for (i = 0; i < iFillCount; ++i) {
		szFill[i] = _paddingCharacter;
	}
	szFill[i] = CharT(0);
	szRet.Insert(0, szFill);
	return szRet;
}

NS_END
