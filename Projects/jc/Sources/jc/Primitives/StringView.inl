/*
 * StringView.inl: BasicStringView<CharT> 멤버 정의.
 * StringView.h 끝에서 include된다 (BasicString.h/.inl 분리와 동일). 직접 include하지 않는다.
 */

#pragma once

#include <cctype>
#include <cwctype>
#include <type_traits>

#include "jc/Container/Vector.h"

NS_JC_BEGIN

template <typename CharT>
inline CharT ViewToLowerChar(CharT _ch)
{
	if constexpr (std::is_same_v<CharT, char>)
	{
		return static_cast<CharT>(tolower(_ch));
	}
	else
	{
		return static_cast<CharT>(towlower(_ch));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
BasicStringView<CharT>::BasicStringView(CharT* _pStr) : pBuf_(_pStr)
{
	if (_pStr)
	{
		len_ = 0;
		while (_pStr[len_] != '\0')
			len_++;
	}
	else
	{
		len_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
BasicStringView<CharT>::BasicStringView(CharT* _pStr, _u32 _len) : pBuf_(_pStr), len_(_len)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
BasicStringView<CharT>::BasicStringView(const StrType& _str) : pBuf_(_str.Source()), len_(_str.Length())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 기본 정보 & 상태 조회
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
const CharT* BasicStringView<CharT>::Source() const
{
	return pBuf_;
}

template <typename CharT>
const CharT* BasicStringView<CharT>::SafeSource() const
{
	static constexpr CharT kEmpty[1] = { CharT(0) };
	return pBuf_ ? pBuf_ : kEmpty;
}

template <typename CharT>
_u32 BasicStringView<CharT>::Length() const
{
	return len_;
}

template <typename CharT>
_u32 BasicStringView<CharT>::LengthWithNull() const
{
	return len_ + 1;
}

template <typename CharT>
bool BasicStringView<CharT>::IsEmpty() const
{
	return len_ == 0;
}

template <typename CharT>
bool BasicStringView<CharT>::IsNull() const
{
	return pBuf_ == nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 검증
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::IsValidIndex(const _s32 _index) const
{
	return _index >= 0 && _index < static_cast<_s32>(len_);
}

template <typename CharT>
bool BasicStringView<CharT>::IsValidIndexRange(const _s32 _startIndex, const _s32 _endIndex) const
{
	return _startIndex >= 0 && _endIndex >= _startIndex && _endIndex < static_cast<_s32>(len_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 문자 접근
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
CharT BasicStringView<CharT>::GetAt(_s32 _idx) const
{
	if (IsValidIndex(_idx))
		return pBuf_[_idx];
	return '\0';
}

template <typename CharT>
CharT BasicStringView<CharT>::First() const
{
	if (len_ > 0)
		return pBuf_[0];
	return '\0';
}

template <typename CharT>
CharT BasicStringView<CharT>::Last() const
{
	if (len_ > 0)
		return pBuf_[len_ - 1];
	return '\0';
}

template <typename CharT>
CharT BasicStringView<CharT>::operator[](_s32 _index) const
{
	if (IsValidIndex(_index))
		return pBuf_[_index];
	return '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////
// 검색 & 찾기 연산
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Find(const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_)
		return -1;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return 0;
	if (searchLen > len_)
		return -1;

	for (_u32 i = 0; i <= len_ - searchLen; i++)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Find(const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Find(const StrType& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Find(_s32 _startIndex, const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_ || _startIndex < 0 || _startIndex >= static_cast<_s32>(len_))
		return -1;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return _startIndex;
	if (_startIndex + searchLen > len_)
		return -1;

	for (_u32 i = _startIndex; i <= len_ - searchLen; i++)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Find(_s32 _startIndex, const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_startIndex, _str.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Find(_s32 _startIndex, const StrType& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_startIndex, _str.Source(), _caseSensitive);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Find(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_ || !IsValidIndexRange(_startIndex, _endIndex))
		return -1;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return _startIndex;

	_u32 rangeLen = _endIndex - _startIndex + 1;
	if (searchLen > rangeLen)
		return -1;

	for (_u32 i = _startIndex; i <= _endIndex - searchLen + 1; i++)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::FindReverse(const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_)
		return -1;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return len_ - 1;
	if (searchLen > len_)
		return -1;

	for (_s32 i = len_ - searchLen; i >= 0; i--)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::FindReverse(const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	return FindReverse(_str.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::FindReverse(const StrType& _str, bool _caseSensitive /*= true*/) const
{
	return FindReverse(_str.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::FindReverse(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_ || !IsValidIndexRange(_startIndex, _endIndex))
		return -1;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return _endIndex;

	_u32 rangeLen = _endIndex - _startIndex + 1;
	if (searchLen > rangeLen)
		return -1;

	for (_s32 i = _endIndex - searchLen + 1; i >= _startIndex; i--)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 패턴 매칭 & 문자열 내용
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::StartWith(const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	if (_str.Length() > len_ || !pBuf_)
		return false;

	const CharT* pSearchStr = _str.Source();
	for (_u32 i = 0; i < _str.Length(); i++)
	{
		CharT a = _caseSensitive ? pBuf_[i] : ViewToLowerChar(pBuf_[i]);
		CharT b = _caseSensitive ? pSearchStr[i] : ViewToLowerChar(pSearchStr[i]);
		if (a != b)
			return false;
	}
	return true;
}

template <typename CharT>
bool BasicStringView<CharT>::StartWith(const StrType& _str, bool _caseSensitive /*= true*/) const
{
	if (_str.Length() > static_cast<_s32>(len_) || !pBuf_)
		return false;

	const CharT* pSearchStr = _str.Source();
	for (_u32 i = 0; i < static_cast<_u32>(_str.Length()); i++)
	{
		CharT a = _caseSensitive ? pBuf_[i] : ViewToLowerChar(pBuf_[i]);
		CharT b = _caseSensitive ? pSearchStr[i] : ViewToLowerChar(pSearchStr[i]);
		if (a != b)
			return false;
	}
	return true;
}

template <typename CharT>
bool BasicStringView<CharT>::EndWith(const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	if (_str.Length() > len_ || !pBuf_)
		return false;

	const CharT* pSearchStr = _str.Source();
	_u32 offset = len_ - _str.Length();
	for (_u32 i = 0; i < _str.Length(); i++)
	{
		CharT a = _caseSensitive ? pBuf_[offset + i] : ViewToLowerChar(pBuf_[offset + i]);
		CharT b = _caseSensitive ? pSearchStr[i] : ViewToLowerChar(pSearchStr[i]);
		if (a != b)
			return false;
	}
	return true;
}

template <typename CharT>
bool BasicStringView<CharT>::EndWith(const StrType& _str, bool _caseSensitive /*= true*/) const
{
	if (_str.Length() > static_cast<_s32>(len_) || !pBuf_)
		return false;

	const CharT* pSearchStr = _str.Source();
	_u32 offset = len_ - _str.Length();
	for (_u32 i = 0; i < static_cast<_u32>(_str.Length()); i++)
	{
		CharT a = _caseSensitive ? pBuf_[offset + i] : ViewToLowerChar(pBuf_[offset + i]);
		CharT b = _caseSensitive ? pSearchStr[i] : ViewToLowerChar(pSearchStr[i]);
		if (a != b)
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::Contain(const CharT* _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str, _caseSensitive) != -1;
}

template <typename CharT>
bool BasicStringView<CharT>::Contain(const BasicStringView& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str, _caseSensitive) != -1;
}

template <typename CharT>
bool BasicStringView<CharT>::Contain(const StrType& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str, _caseSensitive) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 개수 세기 연산
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Count(const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_)
		return 0;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0 || searchLen > len_)
		return 0;

	_s32 count = 0;
	for (_u32 i = 0; i <= len_ - searchLen; i++)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			count++;
			i += searchLen - 1;
		}
	}
	return count;
}

template <typename CharT>
_s32 BasicStringView<CharT>::Count(const BasicStringView& _value, bool _caseSensitive /*= true*/) const
{
	return Count(_value.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Count(const StrType& _value, bool _caseSensitive /*= true*/) const
{
	return Count(_value.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Count(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive /*= true*/) const
{
	if (!_pStr || !pBuf_ || !IsValidIndexRange(_startIndex, _endIndex))
		return 0;

	_u32 searchLen = 0;
	while (_pStr[searchLen] != '\0')
		searchLen++;

	if (searchLen == 0)
		return 0;

	_u32 rangeLen = _endIndex - _startIndex + 1;
	if (searchLen > rangeLen)
		return 0;

	_s32 count = 0;
	for (_u32 i = _startIndex; i <= _endIndex - searchLen + 1; i++)
	{
		bool match = true;
		for (_u32 j = 0; j < searchLen; j++)
		{
			CharT a = _caseSensitive ? pBuf_[i + j] : ViewToLowerChar(pBuf_[i + j]);
			CharT b = _caseSensitive ? _pStr[j] : ViewToLowerChar(_pStr[j]);
			if (a != b)
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			count++;
			i += searchLen - 1;
		}
	}
	return count;
}

template <typename CharT>
_s32 BasicStringView<CharT>::Count(_s32 _startIndex, _s32 _endIndex, const BasicStringView& _value, bool _caseSensitive /*= true*/) const
{
	return Count(_startIndex, _endIndex, _value.Source(), _caseSensitive);
}

template <typename CharT>
_s32 BasicStringView<CharT>::Count(_s32 _startIndex, _s32 _endIndex, const StrType& _value, bool _caseSensitive /*= true*/) const
{
	return Count(_startIndex, _endIndex, _value.Source(), _caseSensitive);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 비교 연산
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Compare(const BasicStringView& _str) const
{
	const CharT* pOther = _str.Source();
	_u32 otherLen = _str.Length();

	_u32 minLen = len_ < otherLen ? len_ : otherLen;
	for (_u32 i = 0; i < minLen; i++)
	{
		if (pBuf_[i] != pOther[i])
			return pBuf_[i] - pOther[i];
	}

	if (len_ < otherLen)
		return -1;
	if (len_ > otherLen)
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Compare(const StrType& _str) const
{
	const CharT* pOther = _str.Source();
	_u32 otherLen = _str.Length();

	_u32 minLen = len_ < otherLen ? len_ : otherLen;
	for (_u32 i = 0; i < minLen; i++)
	{
		if (pBuf_[i] != pOther[i])
			return pBuf_[i] - pOther[i];
	}

	if (len_ < otherLen)
		return -1;
	if (len_ > otherLen)
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::Compare(const CharT* _pStr, _s32 _strLen) const
{
	if (!_pStr)
		return pBuf_ ? 1 : 0;

	_u32 otherLen = 0;
	if (_strLen < 0)
	{
		while (_pStr[otherLen] != '\0')
			otherLen++;
	}
	else
	{
		otherLen = _strLen;
	}

	_u32 minLen = len_ < otherLen ? len_ : otherLen;
	for (_u32 i = 0; i < minLen; i++)
	{
		if (pBuf_[i] != _pStr[i])
			return pBuf_[i] - _pStr[i];
	}

	if (len_ < otherLen)
		return -1;
	if (len_ > otherLen)
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 비교 연산자
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::operator==(const BasicStringView& _other) const
{
	return Compare(_other) == 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator==(const StrType& _other) const
{
	return Compare(_other) == 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator==(const CharT* _pOther) const
{
	return Compare(_pOther) == 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<(const BasicStringView& _other) const
{
	return Compare(_other) < 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<(const StrType& _other) const
{
	return Compare(_other) < 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<(const CharT* _pOther) const
{
	return Compare(_pOther) < 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>(const BasicStringView& _other) const
{
	return Compare(_other) > 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>(const StrType& _other) const
{
	return Compare(_other) > 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>(const CharT* _pOther) const
{
	return Compare(_pOther) > 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<=(const BasicStringView& _other) const
{
	return Compare(_other) <= 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<=(const StrType& _other) const
{
	return Compare(_other) <= 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator<=(const CharT* _pOther) const
{
	return Compare(_pOther) <= 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>=(const BasicStringView& _other) const
{
	return Compare(_other) >= 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>=(const StrType& _other) const
{
	return Compare(_other) >= 0;
}

template <typename CharT>
bool BasicStringView<CharT>::operator>=(const CharT* _pOther) const
{
	return Compare(_pOther) >= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Split 기능
//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
Vector<BasicStringView<CharT>, CDefaultAllocator> BasicStringView<CharT>::Split(const CharT* _delimiter, bool _includeEmpty) const
{
	Vector<BasicStringView<CharT>, CDefaultAllocator> vecTokens;
	
	if (!_delimiter || !pBuf_)
		return vecTokens;

	_u32 delimiterLen = 0;
	while (_delimiter[delimiterLen] != '\0')
		delimiterLen++;

	if (delimiterLen == 0)
	{
		vecTokens.EmplaceBack(*this);
		return vecTokens;
	}

	_s32 offset = Find(_delimiter);
	if (offset == -1)
	{
		vecTokens.EmplaceBack(*this);
		return vecTokens;
	}

	if (offset > 0)
	{
		vecTokens.EmplaceBack(pBuf_, offset);
	}
	else if (_includeEmpty)
	{
		vecTokens.EmplaceBack(nullptr, 0);
	}

	offset += delimiterLen;

	while (offset < static_cast<_s32>(len_))
	{
		_s32 nextOffset = Find(offset, _delimiter);

		if (nextOffset == -1)
			break;

		if (nextOffset > offset)
		{
			vecTokens.EmplaceBack(pBuf_ + offset, nextOffset - offset);
		}
		else if (_includeEmpty)
		{
			vecTokens.EmplaceBack(nullptr, 0);
		}

		offset = nextOffset + delimiterLen;
	}

	if (offset < static_cast<_s32>(len_))
	{
		vecTokens.EmplaceBack(pBuf_ + offset, len_ - offset);
	}
	else if (_includeEmpty)
	{
		vecTokens.EmplaceBack(nullptr, 0);
	}

	return vecTokens;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
Vector<BasicStringView<CharT>, CDefaultAllocator> BasicStringView<CharT>::Split(CharT _delimiter, bool _includeEmpty) const
{
	Vector<BasicStringView<CharT>, CDefaultAllocator> vecTokens;
	
	if (!pBuf_)
		return vecTokens;

	_s32 offset = 0;

	for (_s32 i = 0; i <= (_s32)len_; i++)
	{
		if (i == len_ || pBuf_[i] == _delimiter)
		{
			if (i > offset)
			{
				vecTokens.EmplaceBack(pBuf_ + offset, i - offset);
			}
			else if (_includeEmpty)
			{
				vecTokens.EmplaceBack(nullptr, 0);
			}

			offset = i + 1;
		}
	}

	return vecTokens;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s8 BasicStringView<CharT>::ToInt8(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_u8 BasicStringView<CharT>::ToUInt8(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s16 BasicStringView<CharT>::ToInt16(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_u16 BasicStringView<CharT>::ToUInt16(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s32 BasicStringView<CharT>::ToInt32(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_u32 BasicStringView<CharT>::ToUInt32(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_s64 BasicStringView<CharT>::ToInt64(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_u64 BasicStringView<CharT>::ToUInt64(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_f32 BasicStringView<CharT>::ToFloat(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
_f64 BasicStringView<CharT>::ToDouble(bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::template ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
bool BasicStringView<CharT>::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return BasicStringUtil<CharT>::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////
// 유틸리티 함수들
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
BasicStringView<CharT> BasicStringView<CharT>::SubStr(_s32 _startIdx, _u32 _count) const {
	if (!IsValidIndex(_startIdx))
		return BasicStringView();
	
	_u32 actualCount = _count;
	if (_startIdx + static_cast<_s32>(_count) > static_cast<_s32>(len_))
	{
		actualCount = len_ - _startIdx;
	}

	return BasicStringView(pBuf_ + _startIdx, actualCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
BasicStringView<CharT> BasicStringView<CharT>::GetRange(_s32 _startIdx, _s32 _endIdx) const {
	if (!IsValidIndexRange(_startIdx, _endIdx))
		return BasicStringView();

	_u32 rangeLen = _endIdx - _startIdx + 1;
	return BasicStringView(pBuf_ + _startIdx, rangeLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename BasicStringView<CharT>::StrType BasicStringView<CharT>::ToLowerCase() const {
	StrType str(0);
	if (!pBuf_ || len_ == 0)
		return str;

	CharT* pTemp = Memory::Allocate<CharT*>(static_cast<_u32>((len_ + 1) * sizeof(CharT)));
	for (_u32 i = 0; i < len_; i++)
	{
		pTemp[i] = (pBuf_[i] >= 'A' && pBuf_[i] <= 'Z') ? pBuf_[i] + 32 : pBuf_[i];
	}

	pTemp[len_] = '\0';
	str.ExchangeSource(pTemp, len_);
	return str;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename BasicStringView<CharT>::StrType BasicStringView<CharT>::ToUpperCase() const
{
	StrType str(0);
	if (!pBuf_ || len_ == 0)
		return str;

	CharT* pTemp = Memory::Allocate<CharT*>(static_cast<_u32>((len_ + 1) * sizeof(CharT)));
	for (_u32 i = 0; i < len_; i++)
	{
		pTemp[i] = (pBuf_[i] >= 'a' && pBuf_[i] <= 'z') ? pBuf_[i] - 32 : pBuf_[i];
	}

	pTemp[len_] = '\0';
	str.ExchangeSource(pTemp, len_);
	return str;
}

////////////////////////////////////////////////////////////////////////////////////////
// BasicString glue — BasicStringView가 완성된 뒤라야 정의할 수 있어서 여기 둔다.
template <typename CharT, typename Storage>
inline void BasicString<CharT, Storage>::Append(const BasicStringView<CharT>& _str)
{
	Append(_str.SafeSource(), static_cast<int>(_str.Length()));
}

NS_END
