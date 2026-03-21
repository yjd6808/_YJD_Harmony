#include "jc/Primitives/StringView.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Container/Vector.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
//////////////////////////////////////////////////////////////////////////////////////////
StringView::StringView() : pBuf_(nullptr), len_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
StringView::StringView(char* _pStr) : pBuf_(_pStr)
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
StringView::StringView(char* _pStr, _u32 _len) : pBuf_(_pStr), len_(_len)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
StringView::StringView(const String& _str) : pBuf_(_str.Source()), len_(_str.Length())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 기본 정보 & 상태 조회
//////////////////////////////////////////////////////////////////////////////////////////
const char* StringView::Source() const
{
	return pBuf_;
}

const char* StringView::SafeSource() const
{
	return pBuf_ ? pBuf_ : "";
}

_u32 StringView::Length() const
{
	return len_;
}

_u32 StringView::LengthWithNull() const
{
	return len_ + 1;
}

bool StringView::IsEmpty() const
{
	return len_ == 0;
}

bool StringView::IsNull() const
{
	return pBuf_ == nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 검증
//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::IsValidIndex(const _s32 _index) const
{
	return _index >= 0 && _index < static_cast<_s32>(len_);
}

bool StringView::IsValidIndexRange(const _s32 _startIndex, const _s32 _endIndex) const
{
	return _startIndex >= 0 && _endIndex >= _startIndex && _endIndex < static_cast<_s32>(len_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 문자 접근
//////////////////////////////////////////////////////////////////////////////////////////
char StringView::GetAt(_s32 _idx) const
{
	if (IsValidIndex(_idx))
		return pBuf_[_idx];
	return '\0';
}

char StringView::First() const
{
	if (len_ > 0)
		return pBuf_[0];
	return '\0';
}

char StringView::Last() const
{
	if (len_ > 0)
		return pBuf_[len_ - 1];
	return '\0';
}

char StringView::operator[](_s32 _index) const
{
	if (IsValidIndex(_index))
		return pBuf_[_index];
	return '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////
// 검색 & 찾기 연산
//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringView::Find(const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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
_s32 StringView::Find(const StringView& _str) const
{
	return Find(_str.Source());
}

_s32 StringView::Find(const String& _str) const
{
	return Find(_str.Source());
}

_s32 StringView::Find(_s32 _startIndex, const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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
_s32 StringView::Find(_s32 _startIndex, const StringView& _str) const
{
	return Find(_startIndex, _str.Source());
}

_s32 StringView::Find(_s32 _startIndex, const String& _str) const
{
	return Find(_startIndex, _str.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringView::Find(_s32 _startIndex, _s32 _endIndex, const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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
_s32 StringView::FindReverse(const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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
_s32 StringView::FindReverse(const StringView& _str) const
{
	return FindReverse(_str.Source());
}

_s32 StringView::FindReverse(const String& _str) const
{
	return FindReverse(_str.Source());
}

_s32 StringView::FindReverse(_s32 _startIndex, _s32 _endIndex, const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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
bool StringView::StartWith(const StringView& _str) const
{
	if (_str.Length() > len_ || !pBuf_)
		return false;

	const char* pSearchStr = _str.Source();
	for (_u32 i = 0; i < _str.Length(); i++)
	{
		if (pBuf_[i] != pSearchStr[i])
			return false;
	}
	return true;
}

bool StringView::StartWith(const String& _str) const
{
	if (_str.Length() > static_cast<_s32>(len_) || !pBuf_)
		return false;

	const char* pSearchStr = _str.Source();
	for (_u32 i = 0; i < static_cast<_u32>(_str.Length()); i++)
	{
		if (pBuf_[i] != pSearchStr[i])
			return false;
	}
	return true;
}

bool StringView::EndWith(const StringView& _str) const
{
	if (_str.Length() > len_ || !pBuf_)
		return false;

	const char* pSearchStr = _str.Source();
	_u32 offset = len_ - _str.Length();
	for (_u32 i = 0; i < _str.Length(); i++)
	{
		if (pBuf_[offset + i] != pSearchStr[i])
			return false;
	}
	return true;
}

bool StringView::EndWith(const String& _str) const
{
	if (_str.Length() > static_cast<_s32>(len_) || !pBuf_)
		return false;

	const char* pSearchStr = _str.Source();
	_u32 offset = len_ - _str.Length();
	for (_u32 i = 0; i < static_cast<_u32>(_str.Length()); i++)
	{
		if (pBuf_[offset + i] != pSearchStr[i])
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::Contain(const char* _str) const
{
	return Find(_str) != -1;
}

bool StringView::Contain(const StringView& _str) const
{
	return Find(_str) != -1;
}

bool StringView::Contain(const String& _str) const
{
	return Find(_str) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 개수 세기 연산
//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringView::Count(const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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

_s32 StringView::Count(const StringView& _value) const
{
	return Count(_value.Source());
}

_s32 StringView::Count(const String& _value) const
{
	return Count(_value.Source());
}

_s32 StringView::Count(_s32 _startIndex, _s32 _endIndex, const char* _pStr) const
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
			if (pBuf_[i + j] != _pStr[j])
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

_s32 StringView::Count(_s32 _startIndex, _s32 _endIndex, const StringView& _value) const
{
	return Count(_startIndex, _endIndex, _value.Source());
}

_s32 StringView::Count(_s32 _startIndex, _s32 _endIndex, const String& _value) const
{
	return Count(_startIndex, _endIndex, _value.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 비교 연산
//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringView::Compare(const StringView& _str) const
{
	const char* pOther = _str.Source();
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
_s32 StringView::Compare(const String& _str) const
{
	const char* pOther = _str.Source();
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
_s32 StringView::Compare(const char* _pStr, _s32 _strLen) const
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
bool StringView::operator==(const StringView& _other) const
{
	return Compare(_other) == 0;
}

bool StringView::operator==(const String& _other) const
{
	return Compare(_other) == 0;
}

bool StringView::operator==(const char* _pOther) const
{
	return Compare(_pOther) == 0;
}

bool StringView::operator<(const StringView& _other) const
{
	return Compare(_other) < 0;
}

bool StringView::operator<(const String& _other) const
{
	return Compare(_other) < 0;
}

bool StringView::operator<(const char* _pOther) const
{
	return Compare(_pOther) < 0;
}

bool StringView::operator>(const StringView& _other) const
{
	return Compare(_other) > 0;
}

bool StringView::operator>(const String& _other) const
{
	return Compare(_other) > 0;
}

bool StringView::operator>(const char* _pOther) const
{
	return Compare(_pOther) > 0;
}

bool StringView::operator<=(const StringView& _other) const
{
	return Compare(_other) <= 0;
}

bool StringView::operator<=(const String& _other) const
{
	return Compare(_other) <= 0;
}

bool StringView::operator<=(const char* _pOther) const
{
	return Compare(_pOther) <= 0;
}

bool StringView::operator>=(const StringView& _other) const
{
	return Compare(_other) >= 0;
}

bool StringView::operator>=(const String& _other) const
{
	return Compare(_other) >= 0;
}

bool StringView::operator>=(const char* _pOther) const
{
	return Compare(_pOther) >= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Split 기능
//////////////////////////////////////////////////////////////////////////////////////////
Vector<StringView, CDefaultAllocator> StringView::Split(const char* _delimiter, bool _includeEmpty) const
{
	Vector<StringView, CDefaultAllocator> vecTokens;
	
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
Vector<StringView, CDefaultAllocator> StringView::Split(char _delimiter, bool _includeEmpty) const
{
	Vector<StringView, CDefaultAllocator> vecTokens;
	
	if (!pBuf_)
		return vecTokens;

	_s32 offset = 0;

	for (_u32 i = 0; i <= len_; i++)
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
_s8 StringView::ToInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 StringView::ToUInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 StringView::ToInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 StringView::ToUInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringView::ToInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 StringView::ToUInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 StringView::ToInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 StringView::ToUInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 StringView::ToFloat(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 StringView::ToDouble(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringView::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////
// 유틸리티 함수들
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
StringView StringView::SubStr(_s32 _startIdx, _u32 _count) const {
	if (!IsValidIndex(_startIdx))
		return StringView();
	
	_u32 actualCount = _count;
	if (_startIdx + static_cast<_s32>(_count) > static_cast<_s32>(len_))
	{
		actualCount = len_ - _startIdx;
	}

	return StringView(pBuf_ + _startIdx, actualCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
StringView StringView::GetRange(_s32 _startIdx, _s32 _endIdx) const {
	if (!IsValidIndexRange(_startIdx, _endIdx))
		return StringView();

	_u32 rangeLen = _endIdx - _startIdx + 1;
	return StringView(pBuf_ + _startIdx, rangeLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringView::ToLowerCase() const {
	jc::String str(0);
	if (!pBuf_ || len_ == 0)
		return str;

	char* pTemp = dbg_new char[len_ + 1];
	for (_u32 i = 0; i < len_; i++)
	{
		pTemp[i] = (pBuf_[i] >= 'A' && pBuf_[i] <= 'Z') ? pBuf_[i] + 32 : pBuf_[i];
	}

	pTemp[len_] = '\0';
	str.ExchangeSource(pTemp, len_);
	return str;
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringView::ToUpperCase() const
{
	jc::String str(0);
	if (!pBuf_ || len_ == 0)
		return str;

	char* pTemp = dbg_new char[len_ + 1];
	for (_u32 i = 0; i < len_; i++)
	{
		pTemp[i] = (pBuf_[i] >= 'a' && pBuf_[i] <= 'z') ? pBuf_[i] - 32 : pBuf_[i];
	}

	pTemp[len_] = '\0';
	str.ExchangeSource(pTemp, len_);
	return str;
}
