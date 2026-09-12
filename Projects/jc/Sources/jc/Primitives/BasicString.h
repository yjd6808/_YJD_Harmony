/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * BasicString<CharT, Storage> — 본문 1벌 + Storage 정책 분리 (선언부)
 *
 * - 본문 정의는 BasicString.inl에 있다. String.h 끝에서 include된다 (별칭 이후).
 *   include 순환 방지: 이 헤더는 StringUtil/Exception/StringView를 포함하지 않는다.
 *   (본문에서만 쓰고, 본문은 전부 완성된 뒤에 컴파일된다.)
 * - 본문은 Storage 계약(Data/Len/SetLen/Cap/Reserve/Assign/AssignFresh/Clear/Adopt
 *   + 복사/이동/소멸)만 쓴다. 저장 방식을 가정하는 분기는 금지한다.
 * - 단위 알고리즘(Length/Copy/Compare/Find)은 BasicStringUtil<CharT>가 유일한 집이다.
 * - Format은 char/wchar 둘 다 지원 (vsnprintf/vswprintf). 수치 변환은 char 전용 StringUtil API.
 * - Null 상태 없음. 빈 문자열은 len 0이다. IsNull()은 항상 false.
 * - Reserve는 축소하지 않는다 (Storage 계약).
 * - 이동 대입의 원본 처리 방식은 Storage 재량이다. SSO는 비움(steal), pooled는 swap.
 */

#pragma once

#include <string>
#include <ostream>
#include <type_traits>

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"

NS_JC_BEGIN

class CDefaultAllocator;

template <typename...>
struct Tuple;

template <typename, typename>
class Vector;

template <typename CharT>
class BasicStringView;
template <typename CharT, typename Storage>
class BasicString final
{
	static_assert(std::is_same_v<CharT, typename Storage::value_type>, "Storage::value_type과 CharT가 일치해야 합니다.");

public:
	inline static constexpr int DEFAULT_BUFFER_SIZE = 32;
	inline static constexpr int EXPANDING_FACTOR = 2;

	static const BasicString Empty;

	static const CharT* EmptySource();

	using TStorage = Storage;

	BasicString() = default;
	explicit BasicString(int _capacity);
	BasicString(const CharT* _pStr, int _capacity);
	BasicString(const CharT* _pStr);
	BasicString(CharT _ch, int _count);
	BasicString(const std::basic_string<CharT>& _str);
	BasicString(const BasicString& _str);
	BasicString(BasicString&& _str) noexcept;
	~BasicString() = default;

	CharT* Source() { return st_.Data(); }
	CharT* Source() const { return const_cast<CharT*>(st_.Data()); }
	const CharT* SafeSource() const { return st_.Data(); }
	int Capacity() const { return st_.Cap(); }
	int Length() const { return st_.Len(); }
	int LengthWithNull() const { return st_.Len() + 1; }
	void SetLength(int _length) { st_.SetLen(_length); }
	void ExchangeSource(CharT* _pSrc, int _len);
	void Adopt(CharT* _pBuffer, int _len, int _capacity) { st_.Adopt(_pBuffer, _len, _capacity); }
	bool IsEmpty() const { return st_.Len() == 0; }
	bool IsValidIndex(const int _idx) const { return _idx >= 0 && _idx < st_.Len(); }
	bool IsValidIndexRange(const int _startIdx, const int _endIdx) const
	{
		return _startIdx <= _endIdx && _startIdx >= 0 && _endIdx < st_.Len();
	}

	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	void Append(const T& _str)
	{
		std::string s = std::to_string(_str);
		AppendAscii(s.c_str(), static_cast<int>(s.length()));
	}
	void Append(CharT _ch);
	void Append(const CharT* _pStr);
	void Append(const CharT* _pStr, int _len);
	void Append(CharT* _pStr);
	void Append(const std::basic_string<CharT>& _str);
	void Append(const BasicString& _str);
	void Append(const BasicStringView<CharT>& _str);
	void Append(BasicString&& _str);
	void Append(_s32 _val);
	void Append(_u32 _val);
	void Append(_s32l _val);
	void Append(_u32l _val);
	void Append(_s64 _val);
	void Append(_u64 _val);
	void Append(_s16 _val);
	void Append(_u16 _val);
	// Append(_s8) 없음. _s8 == char 별칭이라 Append(CharT)와 충돌한다.
	// String은 특수화로 회피했지만 비템플릿 Append(char)가 항상 우선이라 dead code였다.
	void Append(_u8 _val);
	void Append(_f32 _val);
	void Append(_f64 _val);
	void Append(_f64l _val);
	void Append(bool _val);

	void Insert(int _idx, const CharT* _pStr);
	void Insert(int _idx, const BasicString& _str);

	void Resize(int _capacity);
	void ResizeIfNeeded(int _length);

	int Compare(const BasicString& _str) const;
	int Compare(const CharT* _pStr, int _strLen = -1) const;
	Vector<int, CDefaultAllocator> FindAll(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true) const;
	Vector<int, CDefaultAllocator> FindAll(const CharT* _pStr, bool _caseSensitive = true) const;
	Vector<int, CDefaultAllocator> FindAll(const BasicString& _str, bool _caseSensitive = true) const;
	int Find(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true) const;
	int Find(int _startIdx, const CharT* _pStr, bool _caseSensitive = true) const;
	int Find(int _startIdx, const BasicString& _str, bool _caseSensitive = true) const;
	int Find(const CharT* _pStr, bool _caseSensitive = true) const;
	int Find(const BasicString& _str, bool _caseSensitive = true) const;
	int FindReverse(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true) const;
	int FindReverse(const BasicString& _str, bool _caseSensitive = true) const;
	int FindReverse(const CharT* _pStr, bool _caseSensitive = true) const;

	bool EndWith(const BasicString& _str, bool _caseSensitive = true) const { return FindReverse(_str.Source(), _caseSensitive) == Length() - _str.Length(); }
	bool StartWith(const BasicString& _str, bool _caseSensitive = true) const { return Find(_str, _caseSensitive) == 0; }

	CharT Last() const { return GetAt(Length() - 1); }
	CharT First() const { return GetAt(0); }

	void Clear();
	void Clear(int _offset, int _length);

	int Count(const CharT* _pStr, bool _caseSensitive = true) const;
	int Count(const BasicString& _value, bool _caseSensitive = true) const;
	int Count(int _startIdx, int _endIdx, const CharT* _pStr, bool _caseSensitive = true) const;
	int Count(int _startIdx, int _endIdx, const BasicString& _value, bool _caseSensitive = true) const;

	int Replace(const CharT* _pFrom, const BasicString& _to, bool _caseSensitive = true);
	int Replace(const BasicString& _from, const BasicString& _to, bool _caseSensitive = true);
	int Replace(int _offset, int _length, const BasicString& _to);
	int Replace(int _offset, const CharT* _pFrom, const BasicString& _to, bool _caseSensitive = true);
	int Replace(int _offset, const BasicString& _from, const BasicString& _to, bool _caseSensitive = true);
	void ReplaceAll(const CharT* _pFrom, const CharT* _pTo, bool _caseSensitive = true);

	bool Contain(const CharT* _pStr) const;
	bool Contain(const BasicString& _str) const;
	void Format(const CharT* _format, ...);

	void SetAt(int _idx, CharT _ch);
	void SetAtForce(int _idx, CharT _ch);
	CharT GetAt(int _idx) const;
	BasicString GetRange(int _startIdx, int _endIdx) const;
	BasicString SubStr(int _startIdx, int _count) const;
	Tuple<CharT*, int, int> GetRangeUnsafe(int _startIdx, int _endIdx) const;
	Vector<BasicString, CDefaultAllocator> Split(const CharT* _delimiter, bool _includeEmpty = false) const;
	Vector<BasicString, CDefaultAllocator> Split(CharT _delimiter, bool _includeEmpty = false) const;

	void Initialize(int _capacity = DEFAULT_BUFFER_SIZE);

	BasicString ToLowerCase() const;
	BasicString ToUpperCase() const;
	int LeadingZeroCount() const;

	std::basic_string<CharT> ToStd() const;

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

	CharT PopBack();

	CharT& operator[](int _idx) const;

	template <typename T>
	BasicString operator+(const T& _other) {
		BasicString temp = *this;
		temp.Append(_other);
		return temp;
	}

	BasicString operator+(const BasicString& _other) const;
	BasicString operator+(CharT _ch) const;
	BasicString operator+(const CharT* _pStr) const;

	template <typename T>
	BasicString& operator+=(const T& _other) { Append(_other); return *this; }
	BasicString& operator+=(const BasicString& _other);
	BasicString& operator+=(CharT _ch);
	BasicString& operator+=(CharT* _pStr);
	BasicString& operator+=(const CharT* _pStr);

	template <typename T>
	BasicString& operator=(const T& _other) {
		Clear();
		Append(_other);
		return *this;
	}

	BasicString& operator=(const BasicString& _other);
	BasicString& operator=(BasicString&& _other) noexcept;
	BasicString& operator=(const CharT* _pOther);
	BasicString& operator=(std::nullptr_t _other);
	bool operator==(const BasicString& _other) const;
	bool operator==(const CharT* _pOther) const;

	bool operator<(const BasicString& _other) const;
	bool operator<(const CharT* _pOther) const;
	bool operator>(const BasicString& _other) const;
	bool operator>(const CharT* _pOther) const;
	bool operator<=(const BasicString& _other) const;
	bool operator<=(const CharT* _pOther) const;
	bool operator>=(const BasicString& _other) const;
	bool operator>=(const CharT* _pOther) const;

	template <typename C, typename S>
	friend std::ostream& operator<<(std::ostream& _os, const BasicString<C, S>& _src);

private:
	void ThrowIfInvalidRangeIndex(int _startIdx, int _endIdx) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(int _idx) const;
	void ReserveForAppend(int _need);
	void CommitLen(int _len);
	void AppendAscii(const char* _pStr, int _len);
	void FormatV(const CharT* _format, va_list _args);
	static CharT ToLowerUnit(CharT _ch);
	static int VSprintfSize(const CharT* _format, va_list _args);
	static int VSprintfWrite(CharT* _pBuf, int _bufCapacity, const CharT* _format, va_list _args);

	Storage st_;
};
NS_END
