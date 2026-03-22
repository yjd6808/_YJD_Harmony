#pragma once

#include "jc/Type.h"
#include "jc/TypeTraits.h"
#include "jc/Define.h"

#include "StringUtil.h"

NS_JC_BEGIN
class String;
class CDefaultAllocator;
template <typename, typename> class Vector;

//////////////////////////////////////////////////////////////////////////////////////////
// StringView - 소유권 없이 문자열 데이터를 읽기 전용으로 보는 클래스
//////////////////////////////////////////////////////////////////////////////////////////
class StringView
{
public:

	constexpr StringView()
	: pBuf_(nullptr), len_(0)
	{
	}

	StringView(char* _pStr);
	StringView(char* _pStr, _u32 _len);
	StringView(const String& _str);

	// StringView s = "abcd"를 할 경우
	// const char[N] -> const char*로 암묵적 처리되어 아래코드로 들어오게 된다.
	// 이를 방지하게 위해 T가 const char*인 경우에만 허용하도록 SFINAE 처리한다.
	template <typename T, DefaultEnableIf_t<std::is_same_v<T, const char*>> = nullptr>
	constexpr StringView(T _pStr)
	{
		pBuf_ = const_cast<char*>(_pStr);
		len_ = StringUtil::CTLength(_pStr);
	}

	template <_u32 N>
	constexpr StringView(const char(&_str)[N])
	{
		pBuf_ = const_cast<char*>(_str);
		len_ = StringUtil::CTLength<N>(_str);
	}

	constexpr StringView(const StringView& _view)
	{
		pBuf_ = _view.pBuf_;
		len_ = _view.len_;
	}

	constexpr StringView& operator=(const StringView& _other)
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
	const char* Source() const;
	const char* SafeSource() const;
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
	char GetAt(_s32 _idx) const;
	char First() const;
	char Last() const;
	char operator[](_s32 _index) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 검색 & 찾기 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Find(const char* _pStr, bool _caseSensitive = true) const;
	_s32 Find(const StringView& _str, bool _caseSensitive = true) const;
	_s32 Find(const String& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const char* _pStr, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const StringView& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, const String& _str, bool _caseSensitive = true) const;
	_s32 Find(_s32 _startIndex, _s32 _endIndex, const char* _pStr, bool _caseSensitive = true) const;

	_s32 FindReverse(const char* _pStr, bool _caseSensitive = true) const;
	_s32 FindReverse(const StringView& _str, bool _caseSensitive = true) const;
	_s32 FindReverse(const String& _str, bool _caseSensitive = true) const;
	_s32 FindReverse(_s32 _startIndex, _s32 _endIndex, const char* _pStr, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 패턴 매칭 & 문자열 내용
	//////////////////////////////////////////////////////////////////////////////////////////
	bool StartWith(const StringView& _str, bool _caseSensitive = true) const;
	bool StartWith(const String& _str, bool _caseSensitive = true) const;

	bool EndWith(const StringView& _str, bool _caseSensitive = true) const;
	bool EndWith(const String& _str, bool _caseSensitive = true) const;

	bool Contain(const char* _str, bool _caseSensitive = true) const;
	bool Contain(const StringView& _str, bool _caseSensitive = true) const;
	bool Contain(const String& _str, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 개수 세기 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Count(const char* _pStr, bool _caseSensitive = true) const;
	_s32 Count(const StringView& _value, bool _caseSensitive = true) const;
	_s32 Count(const String& _value, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const char* _pStr, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const StringView& _value, bool _caseSensitive = true) const;
	_s32 Count(_s32 _startIndex, _s32 _endIndex, const String& _value, bool _caseSensitive = true) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 비교 연산
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 Compare(const StringView& _str) const;
	_s32 Compare(const String& _str) const;
	_s32 Compare(const char* _pStr, _s32 _strLen = -1) const;

	bool operator==(const StringView& _other) const;
	bool operator==(const String& _other) const;
	bool operator==(const char* _pOther) const;

	bool operator<(const StringView& _other) const;
	bool operator<(const String& _other) const;
	bool operator<(const char* _pOther) const;

	bool operator>(const StringView& _other) const;
	bool operator>(const String& _other) const;
	bool operator>(const char* _pOther) const;

	bool operator<=(const StringView& _other) const;
	bool operator<=(const String& _other) const;
	bool operator<=(const char* _pOther) const;

	bool operator>=(const StringView& _other) const;
	bool operator>=(const String& _other) const;
	bool operator>=(const char* _pOther) const;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Split 기능
	//////////////////////////////////////////////////////////////////////////////////////////
	Vector<StringView, CDefaultAllocator> Split(const char* _delimiter, bool _includeEmpty = false) const;
	Vector<StringView, CDefaultAllocator> Split(char _delimiter, bool _includeEmpty = false) const;

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
	StringView SubStr(_s32 _startIdx, _u32 _count) const;
	StringView GetRange(_s32 _startIdx, _s32 _endIdx) const;
	String	   ToLowerCase() const;
	String	   ToUpperCase() const;

private:
	char* pBuf_;
	_u32 len_;
};

NS_END
