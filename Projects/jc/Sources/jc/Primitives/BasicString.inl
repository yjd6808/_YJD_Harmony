/*
 * BasicString.inl: BasicString<CharT, Storage> 본문 정의.
 * AString.h 끝에서 include된다 (별칭 이후). 직접 include하지 않는다.
 * 전부 완성된 뒤라 StringUtil/Exception/AStringView/Memory를 자유롭게 쓴다.
 */

#pragma once

#include <cstdio>
#include <cstdarg>
#include <cwchar>
#include <cwctype>
#include <cstring>

#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Exception.h"
#include "jc/Debug/New.h"
#include "jc/Container/Vector.h"
#include "jc/Tuple.h"
#include "jc/Primitives/StringConvert.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Primitives/StringView.h"

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(int _capacity)
{
	if (_capacity != 0)
	{
		st_.Reserve(_capacity);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(const CharT* _pStr, int _capacity)
{
	if (_pStr == nullptr)
	{
		return;
	}

	const int length = BasicStringUtil<CharT>::Length(_pStr);
	if (length == 0)
	{
		// S1과 동일: 빈 문자열은 capacity 힌트와 무관하게 비워둔다 (힙 할당 0회).
		return;
	}

	// 내용물이 SSO에 들어가면 용량 힌트와 무관하게 SSO로 둔다 (S1 SSO 패리티).
	// Storage::kSsoCapacity = SSO 22 / 슬롯SSO 43. fresh가 아니라 SSO 한계 기준이다.
	if (length <= Storage::kSsoCapacity)
	{
		st_.AssignFresh(_pStr, length);
		return;
	}

	int expectedCapacity = length * EXPANDING_FACTOR;

	if (expectedCapacity < _capacity)
	{
		expectedCapacity = _capacity;
	}

	st_.Reserve(expectedCapacity);
	st_.Assign(_pStr, length);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(const CharT* _pStr)
	: BasicString(_pStr, DEFAULT_BUFFER_SIZE)
{
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(CharT _ch, int _count)
{
	// S1과 동일: 들어가면 SSO 그대로 (+32 없음). 넘어가면 count+32 힙.
	if (_count <= st_.Cap())
	{
		CharT* pBuf = st_.Data();
		for (int i = 0; i < _count; ++i)
		{
			pBuf[i] = _ch;
		}
		CommitLen(_count);
		return;
	}

	st_.Reserve(_count + DEFAULT_BUFFER_SIZE);
	CharT* pBuf = st_.Data();
	for (int i = 0; i < _count; ++i)
	{
		pBuf[i] = _ch;
	}
	CommitLen(_count);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(const std::basic_string<CharT>& _str)
	: BasicString(_str.c_str())
{
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(const BasicString& _str)
	: st_(_str.st_)
{
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>::BasicString(BasicString&& _str) noexcept
	: st_(Move(_str.st_))
{
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ExchangeSource(CharT* _pSrc, int _len)
{
	st_.Adopt(_pSrc, _len, _len);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(CharT _ch)
{
	const int dstLen = Length() + 1;
	ReserveForAppend(dstLen);

	CharT* pBuf = st_.Data();
	pBuf[dstLen - 1] = _ch;
	CommitLen(dstLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(const CharT* _pStr)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	Append(_pStr, BasicStringUtil<CharT>::Length(_pStr));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(const CharT* _pStr, int _len)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	const int oldLen = Length();
	const int dstLen = oldLen + _len;

	// 자기 버퍼를 이어붙이면 Reserve가 원본을 해제할 수 있다. 겹치면 임시 경유.
	const CharT* pBuf = st_.Data();
	if (_pStr >= pBuf && _pStr < pBuf + st_.Cap() + 1)
	{
		CharT* pTemp = Memory::Allocate<CharT*>(static_cast<_u32>((_len + 1) * sizeof(CharT)));
		memcpy(pTemp, _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
		ReserveForAppend(dstLen);
		memcpy(st_.Data() + oldLen, pTemp, static_cast<size_t>(_len) * sizeof(CharT));
		Memory::Deallocate(pTemp);
	}
	else
	{
		ReserveForAppend(dstLen);
		memcpy(st_.Data() + oldLen, _pStr, static_cast<size_t>(_len) * sizeof(CharT));
	}
	CommitLen(dstLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(CharT* _pStr)
{
	Append(const_cast<const CharT*>(_pStr));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(const std::basic_string<CharT>& _str)
{
	if (_str.empty())
	{
		return;
	}

	Append(_str.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(const BasicString& _str)
{
	if (_str.Length() == 0)
	{
		return;
	}

	Append(_str.st_.Data(), _str.st_.Len());
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(BasicString&& _str)
{
	Append(_str.st_.Data(), _str.st_.Len());
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::AppendAscii(const char* _pStr, int _len)
{
	if constexpr (std::is_same_v<CharT, char>)
	{
		Append(_pStr, _len);
	}
	else
	{
		CharT tmp[64];
		jc_assert(_len < 64);
		for (int i = 0; i < _len; ++i)
		{
			tmp[i] = static_cast<CharT>(_pStr[i]);
		}
		Append(tmp, _len);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_s32 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%d", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_u32 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%u", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_s32l _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%ld", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_u32l _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%lu", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_s64 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%lld", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_u64 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%llu", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_s16 _val) { char b[16]; const int n = snprintf(b, sizeof(b), "%hd", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_u16 _val) { char b[16]; const int n = snprintf(b, sizeof(b), "%hu", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_u8 _val) { char b[8]; const int n = snprintf(b, sizeof(b), "%u", static_cast<unsigned int>(_val)); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_f32 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%g", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_f64 _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%g", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage> inline void BasicString<CharT, Storage>::Append(_f64l _val) { char b[32]; const int n = snprintf(b, sizeof(b), "%Lg", _val); if (n > 0) AppendAscii(b, n); }
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(bool _val)
{
	const int len = _val ? 4 : 5;
	AppendAscii(_val ? "true" : "false", len);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Insert(const int _idx, const CharT* _pStr)
{
	const int iLen = BasicStringUtil<CharT>::Length(_pStr);

	if (iLen == 0)
	{
		return;
	}

	if (_idx < 0 || _idx > Length())
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	const int oldLen = Length();
	ReserveForAppend(oldLen + iLen);

	CharT* pBuf = st_.Data();
	if (oldLen - _idx > 0)
	{
		Memory::CopyReverse(
			pBuf + _idx + iLen,
			static_cast<int>((st_.Cap() + 1 - _idx - iLen) * sizeof(CharT)),
			pBuf + _idx,
			static_cast<int>((oldLen - _idx) * sizeof(CharT)));
	}

	for (int i = 0; i < iLen; ++i)
	{
		pBuf[i + _idx] = _pStr[i];
	}

	CommitLen(oldLen + iLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Insert(const int _idx, const BasicString& _str)
{
	Insert(_idx, _str.SafeSource());
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Resize(int _capacity)
{
	st_.Reserve(_capacity);		// Storage 계약: 축소 없음. AString::Resize 축소 의미와 다르다.
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ResizeIfNeeded(int _len)
{
	if (_len > st_.Cap())
	{
		st_.Reserve(_len * EXPANDING_FACTOR);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ReserveForAppend(int _need)
{
	if (_need > st_.Cap())
	{
		st_.Reserve(_need * EXPANDING_FACTOR);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::CommitLen(int _len)
{
	// NUL 종결은 호출자 책임. SetLen은 길이만 둔다 (S1과 동일. Release 비용 0).
	// Debug에서는 종결 누락을 assert로 잡는다.
	st_.Data()[_len] = CharT(0);
	st_.SetLen(_len);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Compare(const BasicString& _str) const
{
	return BasicStringUtil<CharT>::Compare(st_.Data(), st_.Len(), _str.st_.Data(), _str.st_.Len());
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Compare(const CharT* _pStr, const int _strLen) const
{
	const int iStrLen = _strLen == -1 ? BasicStringUtil<CharT>::Length(_pStr) : _strLen;
	return BasicStringUtil<CharT>::Compare(st_.Data(), st_.Len(), _pStr, iStrLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Vector<int, CDefaultAllocator> BasicString<CharT, Storage>::FindAll(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive) const
{
	Vector<int, CDefaultAllocator> offsets;
	const int iStrLen = BasicStringUtil<CharT>::Length(_pStr);

	if (iStrLen == 0)
	{
		return offsets;
	}

	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	for (int i = _startIdx; i <= _endIdx; )
	{
		int iFind = Find(i, _endIdx, _pStr, _caseSensitive);

		if (iFind == -1)
		{
			break;
		}

		offsets.PushBack(iFind);
		i = iFind + iStrLen;
	}

	return offsets;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Vector<int, CDefaultAllocator> BasicString<CharT, Storage>::FindAll(const CharT* _pStr, bool _caseSensitive) const
{
	return FindAll(0, Length() - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Vector<int, CDefaultAllocator> BasicString<CharT, Storage>::FindAll(const BasicString& _str, bool _caseSensitive) const
{
	return FindAll(_str.st_.Data(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Find(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive) const
{
	return BasicStringUtil<CharT>::Find(st_.Data(), st_.Len(), _startIdx, _endIdx, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Find(int _startIdx, const CharT* _pStr, bool _caseSensitive) const
{
	return Find(_startIdx, Length() - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Find(const CharT* _pStr, bool _caseSensitive) const
{
	return Find(0, Length() - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Find(const BasicString& _str, bool _caseSensitive) const
{
	return Find(_str.st_.Data(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Find(int _startIdx, const BasicString& _str, bool _caseSensitive) const
{
	return Find(_startIdx, Length() - 1, _str.SafeSource(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::FindReverse(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive) const
{
	const int iFindStrLen = BasicStringUtil<CharT>::Length(_pStr);
	const int iSrcLen = _endIdx - _startIdx + 1;

	if (iFindStrLen == 0)
	{
		return 0;
	}

	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	if (iFindStrLen > iSrcLen)
	{
		return -1;
	}

	const CharT* pBuf = st_.Data();
	for (int i = _endIdx; i >= _startIdx; i--)
	{
		int iContinuous = 0;

		if (_caseSensitive)
		{
			while (iContinuous < iFindStrLen && pBuf[i + iContinuous] == _pStr[iContinuous])
			{
				iContinuous++;
			}
		}
		else
		{
			while (iContinuous < iFindStrLen && ToLowerUnit(pBuf[i + iContinuous]) == ToLowerUnit(_pStr[iContinuous]))
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
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::FindReverse(const BasicString& _str, bool _caseSensitive) const
{
	return FindReverse(0, Length() - 1, _str.st_.Data(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::FindReverse(const CharT* _pStr, bool _caseSensitive) const
{
	return FindReverse(0, Length() - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline CharT BasicString<CharT, Storage>::ToLowerUnit(CharT _ch)
{
	if constexpr (std::is_same_v<CharT, char>)
	{
		return static_cast<CharT>(tolower(static_cast<unsigned char>(_ch)));
	}
	else
	{
		return static_cast<CharT>(towlower(static_cast<wint_t>(_ch)));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Clear()
{
	st_.Clear();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Clear(int _offset, int _length)
{
	const int len = Length();
	if (_offset >= len)
		return;

	const int iRemoveLen = _offset + _length > len ? len - _offset : _length;
	const int iMoveCharCount = len - _offset - iRemoveLen;

	CharT* pBuf = st_.Data();
	Memory::CopyUnsafe(
		pBuf + _offset,
		pBuf + _offset + iRemoveLen,
		static_cast<int>(iMoveCharCount * sizeof(CharT)));

	st_.SetLen(len - iRemoveLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Count(const CharT* _pStr, bool _caseSensitive) const
{
	return Count(0, Length() - 1, _pStr, _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Count(const BasicString& _value, bool _caseSensitive) const
{
	return Count(0, Length() - 1, _value.SafeSource(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Count(const int _startIdx, const int _endIdx, const CharT* _pStr, bool _caseSensitive) const
{
	ThrowIfNotInitialized();
	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	const int iStrLen = BasicStringUtil<CharT>::Length(_pStr);

	int iOffset = _startIdx;
	int iCount = 0;

	while (iOffset <= _endIdx && (iOffset = Find(iOffset, _endIdx, _pStr, _caseSensitive)) != -1)
	{
		iCount++;
		iOffset += iStrLen;
	}
	return iCount;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Count(const int _startIdx, const int _endIdx, const BasicString& _value, bool _caseSensitive) const
{
	return Count(_startIdx, _endIdx, _value.SafeSource(), _caseSensitive);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Replace(const CharT* _pFrom, const BasicString& _to, bool _caseSensitive)
{
	return Replace(Find(_pFrom, _caseSensitive), BasicStringUtil<CharT>::Length(_pFrom), _to);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Replace(const BasicString& _from, const BasicString& _to, bool _caseSensitive)
{
	return Replace(Find(_from.SafeSource(), _caseSensitive), _from.Length(), _to);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Replace(int _offset, int _length, const BasicString& _to)
{
	if (_offset == -1)
		return -1;

	ThrowIfInvalidIndex(_offset);
	const int len = Length();
	const int iLen = _offset + _length > len ? len - _offset : _length;

	CharT* pBuf = st_.Data();
	if (iLen < _to.Length())
	{
		ReserveForAppend(len + _to.Length() - iLen);
		pBuf = st_.Data();

		Memory::CopyUnsafeReverse(
			pBuf + _offset + _to.Length(),
			pBuf + _offset + iLen,
			static_cast<int>((len - _offset - iLen) * sizeof(CharT))
		);

		for (int i = 0; i < _to.Length(); ++i)
		{
			pBuf[_offset + i] = _to[i];
		}

		CommitLen(len + _to.Length() - iLen);
	}
	else
	{
		Memory::CopyUnsafe(
			pBuf + _offset + _to.Length(),
			pBuf + _offset + iLen,
			static_cast<int>((len - _offset - _to.Length()) * sizeof(CharT))
		);

		for (int i = 0; i < _to.Length(); ++i)
		{
			pBuf[_offset + i] = _to[i];
		}

		CommitLen(len - iLen + _to.Length());
	}

	const int newLen = Length();
	const int iNextOffset = _offset + _to.Length();
	return iNextOffset >= newLen ? -1 : iNextOffset;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Replace(int _offset, const CharT* _pFrom, const BasicString& _to, bool _caseSensitive)
{
	return Replace(Find(_offset, _pFrom, _caseSensitive), BasicStringUtil<CharT>::Length(_pFrom), _to);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::Replace(int _offset, const BasicString& _from, const BasicString& _to, bool _caseSensitive)
{
	return Replace(Find(_offset, _from, _caseSensitive), _from.Length(), _to);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::Contain(const CharT* _pStr) const
{
	return Find(_pStr) != -1;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::Contain(const BasicString& _str) const
{
	return Find(_str.st_.Data()) != -1;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::VSprintfSize(const CharT* _format, va_list _args)
{
	if constexpr (std::is_same_v<CharT, char>)
	{
		return vsnprintf(nullptr, 0, _format, _args);
	}
	else
	{
		return _vscwprintf(_format, _args);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::VSprintfWrite(CharT* _pBuf, int _bufCapacity, const CharT* _format, va_list _args)
{
	if constexpr (std::is_same_v<CharT, char>)
	{
		return vsnprintf(_pBuf, _bufCapacity, _format, _args);
	}
	else
	{
		return vswprintf(_pBuf, static_cast<size_t>(_bufCapacity), _format, _args);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::FormatV(const CharT* _format, va_list _args)
{
	// 길이를 잴 때 va_list를 복사해서 쓴다. 같은 va_list로 두 번 vsnprintf는 UB다.
	va_list argsCopy;
	va_copy(argsCopy, _args);
	const int iExpectedLen = VSprintfSize(_format, argsCopy);
	va_end(argsCopy);

	if (iExpectedLen <= 0)
	{
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	if (iExpectedLen > st_.Cap())
	{
		st_.Reserve(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	VSprintfWrite(st_.Data(), st_.Cap() + 1, _format, _args);
	CommitLen(iExpectedLen);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Format(const CharT* _format, ...)
{
	va_list args;
	va_start(args, _format);
	FormatV(_format, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ReplaceAll(const CharT* _pFrom, const CharT* _pTo, bool _caseSensitive)
{
	const int iFromLen = BasicStringUtil<CharT>::Length(_pFrom);

	if (iFromLen == 0)
	{
		*this = _pTo;
		return;
	}

	BasicString to(_pTo);
	int iReplaceOffset = 0;
	while ((iReplaceOffset = Replace(iReplaceOffset, _pFrom, to, _caseSensitive)) != -1)
	{
		// EMPTY
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::SetAt(const int _idx, const CharT _ch)
{
	ThrowIfInvalidIndex(_idx);
	st_.Data()[_idx] = _ch;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::SetAtForce(int _idx, CharT _ch)
{
	if (_idx < 0 || _idx >= st_.Cap())
		return;
	st_.Data()[_idx] = _ch;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline CharT BasicString<CharT, Storage>::GetAt(const int _idx) const
{
	return st_.Data()[_idx];
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::GetRange(const int _startIdx, const int _endIdx) const
{
	if (_startIdx > _endIdx || _startIdx < 0 || _endIdx >= Length())
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	BasicString subString;
	subString.Append(st_.Data() + _startIdx, _endIdx - _startIdx + 1);
	return subString;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::SubStr(int _startIdx, int _count) const
{
	ThrowIfInvalidIndex(_startIdx);
	if (_count < 0)
	{
		throw InvalidArgumentException("부분 문자열의 길이는 0 이상이어야 합니다.");
	}
	if (_startIdx + _count > Length())
	{
		_count = Length() - _startIdx;
	}
	return GetRange(_startIdx, _startIdx + _count - 1);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Tuple<CharT*, int, int> BasicString<CharT, Storage>::GetRangeUnsafe(const int _startIdx, const int _endIdx) const
{
	if (_startIdx > _endIdx || _startIdx < 0 || _endIdx >= Length())
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	const int iAllocCapacity = _endIdx - _startIdx + 10;
	CharT* szRange = dbg_new CharT[iAllocCapacity];

	for (int i = _startIdx, j = 0; i <= _endIdx; ++i, ++j)
	{
		szRange[j] = st_.Data()[i];
	}
	szRange[_endIdx - _startIdx + 1] = CharT(0);
	return { szRange, _endIdx - _startIdx + 1, iAllocCapacity };
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Vector<BasicString<CharT, Storage>, CDefaultAllocator> BasicString<CharT, Storage>::Split(const CharT* _delimiter, const bool _includeEmpty) const
{
	Vector<BasicString, CDefaultAllocator> vecTokens;
	int iOffset = Find(_delimiter);

	if (iOffset == -1)
	{
		vecTokens.EmplaceBack(st_.Data());
		return vecTokens;
	}

	const int iDelimiterLen = BasicStringUtil<CharT>::Length(_delimiter);
	const int len = Length();
	if (iOffset - 1 < 0)
	{
		if (_includeEmpty)
		{
			vecTokens.EmplaceBack(EmptySource());
		}
	}
	else
	{
		vecTokens.EmplaceBack(GetRange(0, iOffset - 1));
	}

	iOffset += iDelimiterLen;

	while (iOffset < len)
	{
		const int iNextOffset = Find(iOffset, len - 1, _delimiter);

		if (iNextOffset == -1)
		{
			break;
		}

		if (iNextOffset <= iOffset)
		{
			if (_includeEmpty)
			{
				vecTokens.EmplaceBack(EmptySource());
			}
		}
		else
		{
			vecTokens.EmplaceBack(GetRange(iOffset, iNextOffset - 1));
		}
		iOffset = iNextOffset + 1;
	}

	if (iOffset < len)
	{
		vecTokens.EmplaceBack(GetRange(iOffset, len - 1));
	}
	else
	{
		if (_includeEmpty)
		{
			vecTokens.EmplaceBack(EmptySource());
		}
	}

	return vecTokens;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline Vector<BasicString<CharT, Storage>, CDefaultAllocator> BasicString<CharT, Storage>::Split(CharT _delimiter, const bool _includeEmpty) const
{
	Vector<BasicString, CDefaultAllocator> vecTokens;
	int iOffset = 0;
	const int len = Length();
	const CharT* pBuf = st_.Data();

	for (int i = 0; i <= len; i++)
	{
		if (i == len || pBuf[i] == _delimiter)
		{
			if (i > iOffset)
			{
				vecTokens.EmplaceBack(GetRange(iOffset, i - 1));
			}
			else if (_includeEmpty)
			{
				vecTokens.EmplaceBack(EmptySource());
			}

			iOffset = i + 1;
		}
	}

	return vecTokens;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline const CharT* BasicString<CharT, Storage>::EmptySource()
{
	static constexpr CharT kEmpty[1] = { CharT(0) };
	return kEmpty;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Initialize(int _capacity)
{
	st_.Clear();
	if (_capacity > 0)
	{
		st_.Reserve(_capacity);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::ToLowerCase() const
{
	BasicString copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		CharT ch = copy[i];
		if (ch >= CharT('A') && ch <= CharT('Z'))
		{
			copy[i] = static_cast<CharT>(ch + 32);
		}
	}

	return copy;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::ToUpperCase() const
{
	BasicString copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		CharT ch = copy[i];
		if (ch >= CharT('a') && ch <= CharT('z'))
		{
			copy[i] = static_cast<CharT>(ch - 32);
		}
	}

	return copy;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline int BasicString<CharT, Storage>::LeadingZeroCount() const
{
	int iCount = 0;
	const CharT* pBuf = st_.Data();
	for (int i = 0; i < Length(); ++i)
	{
		if (pBuf[i] == CharT('0'))
			++iCount;
		else
			break;
	}

	return iCount;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline std::basic_string<CharT> BasicString<CharT, Storage>::ToStd() const
{
	return st_.Data();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline CharT& BasicString<CharT, Storage>::operator[](const int _idx) const
{
	ThrowIfInvalidIndex(_idx);
	// AString과 동일하게 const 메서드에서도 원소에 쓰기 가능 (원본 quirk 그대로).
	return const_cast<CharT&>(st_.Data()[_idx]);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::operator+(const BasicString& _other) const
{
	BasicString temp = *this;
	temp.Append(_other);
	return temp;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::operator+(CharT _ch) const
{
	BasicString temp = *this;
	temp.Append(_ch);
	return temp;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage> BasicString<CharT, Storage>::operator+(const CharT* _pStr) const
{
	BasicString temp = *this;
	temp.Append(_pStr);
	return temp;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator+=(const BasicString& _other)
{
	Append(_other);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator+=(CharT _ch)
{
	Append(_ch);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator+=(CharT* _pStr)
{
	Append(_pStr);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator+=(const CharT* _pStr)
{
	Append(_pStr);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator=(const BasicString& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	st_ = _other.st_;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator=(BasicString&& _other) noexcept
{
	st_ = Move(_other.st_);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator=(const CharT* _pOther)
{
	// S1과 동일: 모자랄 때만 정리 후 확보. 들어가면 SSO로 (힙 해제), 넘치면 len+10+32 힙.
	// SSO 한계는 Storage::kSsoCapacity로 본다 (현재 Cap이 아니라).
	const int iToLen = BasicStringUtil<CharT>::Length(_pOther);
	const int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > st_.Cap())
	{
		st_.Clear();
		if (iToLen > Storage::kSsoCapacity)
		{
			st_.Reserve(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
		}
	}

	st_.Assign(_pOther, iToLen);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline BasicString<CharT, Storage>& BasicString<CharT, Storage>::operator=(std::nullptr_t _other)
{
	st_.Clear();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator==(const BasicString& _other) const
{
	return Compare(_other) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator==(const CharT* _pOther) const
{
	return Compare(_pOther) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator<(const BasicString& _other) const
{
	return Compare(_other) < 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator<(const CharT* _pOther) const
{
	return Compare(_pOther) < 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator>(const BasicString& _other) const
{
	return Compare(_other) > 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator>(const CharT* _pOther) const
{
	return Compare(_pOther) > 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator<=(const BasicString& _other) const
{
	return Compare(_other) <= 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator<=(const CharT* _pOther) const
{
	return Compare(_pOther) <= 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator>=(const BasicString& _other) const
{
	return Compare(_other) >= 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::operator>=(const CharT* _pOther) const
{
	return Compare(_pOther) >= 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ThrowIfInvalidRangeIndex(int _startIdx, int _endIdx) const
{
	if (!IsValidIndexRange(_startIdx, _endIdx))
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ThrowIfNotInitialized() const
{
	if (st_.Data() == nullptr)
	{
		throw NullPointerException("String을 먼저 초기화해주세요.");
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::ThrowIfInvalidIndex(int _idx) const
{
	if (!IsValidIndex(_idx))
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_s8 BasicString<CharT, Storage>::ToInt8(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_u8 BasicString<CharT, Storage>::ToUInt8(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_s16 BasicString<CharT, Storage>::ToInt16(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_u16 BasicString<CharT, Storage>::ToUInt16(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_s32 BasicString<CharT, Storage>::ToInt32(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_u32 BasicString<CharT, Storage>::ToUInt32(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_s64 BasicString<CharT, Storage>::ToInt64(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_u64 BasicString<CharT, Storage>::ToUInt64(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_f32 BasicString<CharT, Storage>::ToFloat(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
_f64 BasicString<CharT, Storage>::ToDouble(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline bool BasicString<CharT, Storage>::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Storage>
inline CharT BasicString<CharT, Storage>::PopBack()
{
	const int len = Length();
	if (len == 0)
		return CharT(0);

	CharT popped = st_.Data()[len - 1];
	CommitLen(len - 1);
	return popped;
}

////////////////////////////////////////////////////////////////////////////////////////
// 클래스 템플릿 정적 멤버 정의 (헤더-only. vague linkage로 링커가 병합)
template <typename CharT, typename Storage>
const BasicString<CharT, Storage> BasicString<CharT, Storage>::Empty{};

////////////////////////////////////////////////////////////////////////////////////////
// narrow ostream 출력 (gtest 실패 메시지용). wide는 UTF-8로 변환한다.
template <typename CharT, typename Storage>
std::ostream& operator<<(std::ostream& _os, const BasicString<CharT, Storage>& _src)
{
	if constexpr (std::is_same_v<CharT, char>)
		_os << _src.Source();
	else
		_os << StringConvert::ToUtf8(_src.Source(), _src.Length()).Source();
	return _os;
}

NS_END
