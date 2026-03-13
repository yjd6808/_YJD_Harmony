/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#include <jc/Core.h>
#include <jc/Primitives/String.h>
#include <jc/Primitives/StringUtil.h>
#include <jc/Exception.h>
#include <jc/Container/Vector.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String, CDefaultAllocator> StringUtil::Split(String& _src, const char* _pDelimiter)
{
	return _src.Split(_pDelimiter);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String, CDefaultAllocator> StringUtil::Split(String& _src, char _delimiter)
{
	return _src.Split(_delimiter);
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringUtil::Format(const char* _pFormat, ...)
{
	va_list args;
	va_start(args, _pFormat);
	String szResult = Format(_pFormat, args);
	va_end(args);
	return szResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringUtil::Format(const char* _pFormat, va_list _args)
{
	const int iExpectedLen = vsnprintf(nullptr, 0, _pFormat, _args);

	if (iExpectedLen <= 0)
	{
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	String szResult(iExpectedLen + 1 + String::DEFAULT_BUFFER_SIZE);
	vsnprintf(szResult.Source(), szResult.Capacity(), _pFormat, _args);
	szResult.SetAtForce(iExpectedLen, NULL);
	szResult.len_ = iExpectedLen;
	return szResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::FormatBuffer(char* _pBuff, const int _buffCapacity, const char* _pFormat, ...)
{
	va_list args;
	va_start(args, _pFormat);
	FormatBuffer(_pBuff, _buffCapacity, _pFormat, args);
	va_end(args);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::FormatBuffer(char* _pBuff, const int _buffCapacity, const char* _pFormat, va_list _args)
{
	const int iExpectedLen = vsnprintf(nullptr, 0, _pFormat, _args);
	jc_assert_msg(iExpectedLen > 0, "문자열 포맷 수행중 오류가 발생하였습니다.");
	jc_assert_msg(iExpectedLen < _buffCapacity, "문자열 포맷 수행중 오류가 발생하였습니다. (문자열 길이가 버퍼의 용량을 초과합니다.)");
	vsnprintf(_pBuff, _buffCapacity, _pFormat, _args);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringUtil::IsNullOrEmpty(const char* _pStr)
{
	if (_pStr == nullptr)
		return true;
	if (_pStr[0] == '\0')
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::Copy(char* _pBuffer, const int _bufferSize, const char* _pCopy)
{
	if (_pBuffer == nullptr || _pCopy == nullptr)
	{
		return -1;
	}

	int iSize = 0;
	char* pBuffer = _pBuffer;

	while (*_pCopy != NULL && iSize < _bufferSize)
	{
		*_pBuffer = *_pCopy;
		_pBuffer++;
		_pCopy++;
		iSize++;
	}

	pBuffer[iSize] = NULL;
	return iSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::CopyUnsafe(char* _pBuffer, const char* _pCopy)
{
	if (_pBuffer == nullptr || _pCopy == nullptr)
	{
		return -1;
	}

	int iSize = 0;
	char* pBuffer = _pBuffer;

	while (*_pCopy != NULL)
	{
		*_pBuffer = *_pCopy;
		_pBuffer++;
		_pCopy++;
		iSize++;
	}

	pBuffer[iSize] = NULL;
	return iSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringUtil::IsEqual(const char* _pSrc, const char* _pDst, bool _bCompareCase/*= true*/)
{
	return IsEqual(_pSrc, Length(_pSrc), _pDst, Length(_pDst), _bCompareCase);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringUtil::IsEqual(const char* _pSrc, const int _srcLen, const char* _pDst, const int _dstLen, bool _bCompareCase /*= true*/)
{
	for (int i = 0, j = 0; i < _srcLen && j < _dstLen; i++, j++)
	{
		if (_bCompareCase == false)
		{
			if (tolower(*(_pSrc + i)) != tolower(*(_pDst + j)))
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

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::Swap(String& _src, String& _dst)
{
	String temp = std::move(_src);
	_src = std::move(_dst);
	_dst = std::move(temp);
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::Find(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr)
{
	return Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, Length(_pStr));
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* StringUtil::SkipLeadingChar(const char* _pStr, char _skipChar)
{
	const int iPos = FindCharUncontained(_pStr, _skipChar);
	return _pStr + iPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* StringUtil::SkipLeadingNumberZero(const char* _pStr)
{
	const int iStrLen = Length(_pStr);

	if (iStrLen == 0)
	{
		return _pStr;
	}

	const char* szSkippedLeadingZero = SkipLeadingChar(_pStr, '0');
	if (szSkippedLeadingZero == _pStr + iStrLen)
	{
		return "0";
	}

	return szSkippedLeadingZero;
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindChar(const char* _pSource, char _ch)
{
	int i = 0;

	while (true)
	{
		const char sch = *(_pSource + i);

		if (sch == NULL)
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

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindCharReverse(const char* _pSource, char _ch)
{
	return FindCharReverse(_pSource, Length(_pSource), _ch);
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindCharReverse(const char* _pSource, int _len, char _ch)
{
	while ((--_len) >= 0)
	{
		if (_pSource[_len] == _ch)
			return _len;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindCharUncontained(const char* _pSource, char _ch)
{
	int i = 0;

	while (true)
	{
		const char sch = *(_pSource + i);

		if (sch == NULL)
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

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::Find(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr, int _strLen)
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

		while (iContinuous < iFindStrLen && _pSource[i + iContinuous] == _pStr[iContinuous])
		{
			iContinuous++;
		}

		if (iContinuous == iFindStrLen)
		{
			return i;
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::Find(const char* _pSource, int _sourceLen, int _startIdx, const char* _pStr)
{
	return Find(_pSource, _sourceLen, _startIdx, _sourceLen - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindAll(OUT int* _pPositionArray, const char* _pSource, const char* _pStr)
{
	const int iSourceLength = Length(_pSource);
	return FindAll(_pPositionArray, _pSource, iSourceLength, 0, iSourceLength - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindAll(int* _pPositionArray, const char* _pSource, int _sourceLen, const char* _pStr)
{
	return FindAll(_pPositionArray, _pSource, _sourceLen, 0, _sourceLen - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int StringUtil::FindAll(int* _pPositionArray, const char* _pSource, int _sourceLen, int _startIdx, int _endIdx, const char* _pStr)
{
	if (_endIdx < _startIdx)
	{
		return 0;
	}

	int iCount = 0;
	int iFindPos = -1;
	int iStrLength = Length(_pStr);

	while ((iFindPos = Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, iStrLength)) != -1)
	{
		_pPositionArray[iCount++] = iFindPos;
		_startIdx = iFindPos + iStrLength;
	}

	return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringUtil::GetRange(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx)
{
	String subString{0};

	auto [pBuffer, iLen, iCapacity] = GetRangeUnsafe(_pSource, _sourceLen, _startIdx, _endIdx);

	subString.pBuffer_ = pBuffer;
	subString.len_ = iLen;
	subString.capacity_ = iCapacity;

	return subString;
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringUtil::SubStr(const char* _pSource, int _sourceLen, int _startIdx, int _count)
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

//////////////////////////////////////////////////////////////////////////////////////////
Tuple<char*, int, int> StringUtil::GetRangeUnsafe(const char* _pSource, int _sourceLen, int _startIdx, int _endIdx)
{
	if (_startIdx > _endIdx || _startIdx < 0 || _endIdx >= _sourceLen)
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	const char* pStr = _pSource + _startIdx;
	int iCurIdx = _startIdx;
	int iIdx = 0;
	const int iAllocCapacity = _endIdx - _startIdx + 10;
	char* szRange = dbg_new char[iAllocCapacity];

	while (iCurIdx <= _endIdx)
	{
		szRange[iIdx] = _pSource[iCurIdx];
		iCurIdx++;
		iIdx++;
	}

	szRange[iIdx] = NULL;
	return { szRange, iIdx, iAllocCapacity };
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::ConcatInnerBack(char* _pBuf, int _buflen, int _bufCapacity, const char* _pConcatStr, int _concatStrLen)
{
	jc_assert_msg(_buflen + _concatStrLen + 1 <= _bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafe(_pBuf + _buflen, _pConcatStr, _concatStrLen);
	_pBuf[_buflen + _concatStrLen] = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::ConcatInnerBack(char* _pBuf, int _bufCapacity, const char* _pConcatStr)
{
	int iBufLen = Length(_pBuf);
	int iConcatLen = Length(_pConcatStr);
	ConcatInnerBack(_pBuf, iBufLen, _bufCapacity, _pConcatStr, iConcatLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::ConcatInnerFront(char* _pBuf, int _buflen, int _bufCapacity, const char* _pConcatStr, int _concatStrLen)
{
	jc_assert_msg(_buflen + _concatStrLen + 1 <= _bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafeReverse(_pBuf + _concatStrLen, _pBuf, _buflen);
	Memory::CopyUnsafe(_pBuf, _pConcatStr, _concatStrLen);
	_pBuf[_buflen + _concatStrLen] = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringUtil::ConcatInnerFront(char* _pBuf, int _bufCapacity, const char* _pConcatStr)
{
	int iBufLen = Length(_pBuf);
	int iConcatLen = Length(_pConcatStr);
	ConcatInnerFront(_pBuf, iBufLen, _bufCapacity, _pConcatStr, iConcatLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> float StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtof(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> double StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtod(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s32l StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtol(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s64 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtoll(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _u32l StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtoul(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _u64 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return std::strtoull(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s8 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_s8>(std::strtol(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s16c StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_s16c>(std::strtoul(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _u8 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_u8>(std::strtoul(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s16 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_s16>(std::strtol(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _u16 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_u16>(std::strtoul(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _s32 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_s32>(std::strtol(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> _u32 StringUtil::ToNumber(const char* _pStr, OUT char** _ppEndptr /* = nullptr */, bool _ignoreLeadingZero /* = true */ )
{
	return static_cast<_u32>(std::strtoul(_ignoreLeadingZero ? SkipLeadingNumberZero(_pStr) : _pStr, _ppEndptr, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(bool _value)
{
	return _value ? "true" : "false";
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(float _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(double _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_s32l _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_s64 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_u32l _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_u64 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_s8 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_u8 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_s16 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_u16 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_s32 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(_u32 _value)
{
	return std::to_string(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(const String& _value)
{
	return _value;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <> String StringUtil::ToString(String&& _value)
{
	return std::move(_value);
}

NS_END
