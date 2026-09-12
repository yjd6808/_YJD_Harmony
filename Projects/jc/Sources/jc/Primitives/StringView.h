#pragma once

#include <type_traits>

#include "jc/Type.h"
#include "jc/TypeTraits.h"
#include "jc/Define.h"

#include "StringUtil.h"
#include "jc/Primitives/String.h"

NS_JC_BEGIN
class CDefaultAllocator;
template <typename, typename> class Vector;

////////////////////////////////////////////////////////////////////////////////////////
// BasicStringView - 소유권 없이 문자열 데이터를 읽기 전용으로 보는 클래스
////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT = char>
class BasicStringView
{
public:
	using StrType = BasicString<CharT, StringImpl_SSO<CharT>>;

	constexpr BasicStringView()
	: pBuf_(nullptr), len_(0)
	{
	}

	BasicStringView(CharT* _pStr);
	BasicStringView(CharT* _pStr, _u32 _len);
	BasicStringView(const StrType& _str);

	template <_u32 N>
	constexpr BasicStringView(const CharT(&_str)[N])
	{
		pBuf_ = const_cast<CharT*>(_str);
		len_ = N - 1;
	}

	constexpr BasicStringView(const BasicStringView& _view)
	{
		pBuf_ = _view.pBuf_;
		len_ = _view.len_;
	}

	constexpr BasicStringView& operator=(const BasicStringView& _other)
	{
		if (this == &_other)
		{
			return *this;
		}
		pBuf_ = _other.pBuf_;
		len_ = _other.len_;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 기본 정보 & 상태 조회
	//////////////////////////////////////////////////////////////////////////////////////////
	const CharT* Source() const;
	const CharT* SafeSource() const;
	_u32* LengthPtr() { return &len_; }
	_u32 Length() const;
	_u32 LengthWithNull() const;
	bool IsEmpty() const;
	bool IsNull() const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 인덱스 검증
	//////////////////////////////////////////////////////////////////////////////////////////
	bool IsValidIndex(const _s32 _index) const;
	bool IsValidIndexRange(const _s32 _startIndex, const _s32 _endIndex) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 문자 접근
	//////////////////////////////////////////////////////////////////////////////////////////
	CharT GetAt(_s32 _idx) const;
	CharT First() const;
	CharT Last() const;
	CharT operator[](_s32 _index) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 검색 & 찾기 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Find(const CharT* _pStr, bool _caseSensitive = true) const;
	_s32 Find(const BasicStringView& _str, bool _caseSensitive = true) const;
	_s32 Find(const StrType& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const CharT* _pStr, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const BasicStringView& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const StrType& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive = true) const;

	_s32 FindReverse(const CharT* _pStr, bool _caseSensitive = true) const;
	_s32 FindReverse(const BasicStringView& _str, bool _caseSensitive = true) const;
	_s32 FindReverse(const StrType& _str, bool _caseSensitive = true) const;
	_s32 FindReverse(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 패턴 매칭 & 문자열 내용
	//////////////////////////////////////////////////////////////////////////////////////////
	bool StartWith(const BasicStringView& _str, bool _caseSensitive = true) const;
	bool StartWith(const StrType& _str, bool _caseSensitive = true) const;

	bool EndWith(const BasicStringView& _str, bool _caseSensitive = true) const;
	bool EndWith(const StrType& _str, bool _caseSensitive = true) const;

	bool Contain(const CharT* _str, bool _caseSensitive = true) const;
	bool Contain(const BasicStringView& _str, bool _caseSensitive = true) const;
	bool Contain(const StrType& _str, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 개수 세기 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Count(const CharT* _pStr, bool _caseSensitive = true) const;
	_s32 Count(const BasicStringView& _value, bool _caseSensitive = true) const;
	_s32 Count(const StrType& _value, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const CharT* _pStr, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const BasicStringView& _value, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const StrType& _value, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 비교 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Compare(const BasicStringView& _str) const;
	_s32 Compare(const StrType& _str) const;
	_s32 Compare(const CharT* _pStr, _s32 _strLen = -1) const;

	bool operator==(const BasicStringView& _other) const;
	bool operator==(const StrType& _other) const;
	bool operator==(const CharT* _pOther) const;

	bool operator<(const BasicStringView& _other) const;
	bool operator<(const StrType& _other) const;
	bool operator<(const CharT* _pOther) const;

	bool operator>(const BasicStringView& _other) const;
	bool operator>(const StrType& _other) const;
	bool operator>(const CharT* _pOther) const;

	bool operator<=(const BasicStringView& _other) const;
	bool operator<=(const StrType& _other) const;
	bool operator<=(const CharT* _pOther) const;

	bool operator>=(const BasicStringView& _other) const;
	bool operator>=(const StrType& _other) const;
	bool operator>=(const CharT* _pOther) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Split 기능
	//////////////////////////////////////////////////////////////////////////////////////////
	Vector<BasicStringView, CDefaultAllocator> Split(const CharT* _delimiter, bool _includeEmpty = false) const;
	Vector<BasicStringView, CDefaultAllocator> Split(CharT _delimiter, bool _includeEmpty = false) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 타입 변환 함수 - StringUtil의 ToNumber/TryToNumber를 활용
	//////////////////////////////////////////////////////////////////////////////////////////
	_s8 ToInt8(bool _ignoreLeadingZero = true) const;
	_u8 ToUInt8(bool _ignoreLeadingZero = true) const;
	_s16 ToInt16(bool _ignoreLeadingZero = true) const;
	_u16 ToUInt16(bool _ignoreLeadingZero = true) const;
	_s32 ToInt32(bool _ignoreLeadingZero = true) const;
	_u32 ToUInt32(bool _ignoreLeadingZero = true) const;
	_s64 ToInt64(bool _ignoreLeadingZero = true) const;
	_u64 ToUInt64(bool _ignoreLeadingZero = true) const;
	_f32 ToFloat(bool _ignoreLeadingZero = true) const;
	_f64 ToDouble(bool _ignoreLeadingZero = true) const;

	bool TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 유틸리티 함수들
	//////////////////////////////////////////////////////////////////////////////////////////
	BasicStringView SubStr(_s32 _startIdx, _u32 _count) const;
	BasicStringView GetRange(_s32 _startIdx, _s32 _endIdx) const;
	StrType	   ToLowerCase() const;
	StrType	   ToUpperCase() const;

private:
	CharT* pBuf_;
	_u32 len_;
};

using StringView = BasicStringView<char>;
using AStringView = BasicStringView<char>;
using WStringView = BasicStringView<wchar_t>;
using StringViewT = BasicStringView<_char>;

NS_END

#include "jc/Primitives/StringView.inl"
