/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/

#pragma once

#include <string>
#include <sstream>
#include <ostream>

#include <jc/Namespace.h>
#include <jc/Type.h>
#include <jc/Define.h>

NS_JC_BEGIN

// variadic template forward declaration
template <typename...>
struct Tuple;
class CDefaultAllocator;

template <typename, typename>
class Vector;

class String final
{
	inline static constexpr int DEFAULT_BUFFER_SIZE = 32;	// 초기 버퍼의 크기
	inline static constexpr int EXPANDING_FACTOR = 2;		// 더해준 문자열과 기존 문자열의 길이를 합쳤을 때 용량을 초과할 경우 배열 크기를 몇배 확장시켜줄지

	inline static const char* EmptySource = "";
public:
	static const String Empty;
	static const String Null;

	String();
	explicit String(int _capacity);
	String(const char* _pStr, int _capacity);
	String(const char* _pStr);
	String(char _ch, int _count);
	String(const std::string& _str);
	String(const String& _str);
	String(String&& _str) noexcept;
	~String();
public:
	char* Source() const { return pBuffer_; }
	const char* SafeSource() const { return pBuffer_ != nullptr ? pBuffer_ : EmptySource; }
	int Capacity() const { return capacity_; }
	int Length() const { return len_; }
	int LengthWithNull() const { return len_ + 1; }
	void SetLength(int _length) { len_ = _length; }
	void ExchangeSource(char* _pSrc, int _len);
	bool IsEmpty() const { return len_ == 0; }
    bool IsNull() const { return pBuffer_ == nullptr; }
	bool IsValidIndex(const int _idx) const { return _idx >= 0 && _idx < len_; }
	bool IsValidIndexRange(const int _startIdx, const int _endIdx) const 
	{
		return _startIdx <= _endIdx && _startIdx >= 0 && _endIdx < len_;
	}

	template <typename T>
	void Append(const T& _str);
	void Append(char _ch);
	void Append(const char* _pStr);
	void Append(const char* _pStr, int _len);
	void Append(char* _pStr);
	void Append(const std::string& _str);
	void Append(const String& _str);
	void Append(String&& _str);

	void Insert(int _idx, const char* _pStr);
	void Insert(int _idx, const String& _str);

	void Resize(int _capacity);
	void ResizeIfNeeded(int _length);		// len이 m_iCapcity 이상일 경우에 x2해서 확장

	int Compare(const String& _str) const;
	int Compare(const char* _pStr, int _strLen = -1) const;
	Vector<int, CDefaultAllocator> FindAll(int _startIdx, int _endIdx, const char* _pStr) const;
	Vector<int, CDefaultAllocator> FindAll(const char* _pStr) const;
	Vector<int, CDefaultAllocator> FindAll(const String& _str) const;
	int Find(int _startIdx, int _endIdx, const char* _pStr) const;
	int Find(int _startIdx, const char* _pStr) const;
	int Find(int _startIdx, const String& _str) const;
	int Find(const char* _pStr) const;
	int Find(const String& _str) const;
	int FindReverse(int _startIdx, int _endIdx, const char* _pStr) const;
	int FindReverse(const String& _str) const;
	int FindReverse(const char* _pStr) const;

	bool EndWith(const String& _str) const { return FindReverse(_str.Source()) == len_ - _str.Length();  }
	bool StartWith(const String& _str) const { return Find(_str) == 0; }

	char Last() const { return GetAt(len_ - 1); }
	char First() const { return GetAt(0); }

	void Clear();
	void Clear(int _offset, int _length);		// offset 인덱스에서 len만큼 없앰

	int Count(const char* _pStr) const;
	int Count(const String& _value) const;
	int Count(int _startIdx, int _endIdx, const char* _pStr) const;
	int Count(int _startIdx, int _endIdx, const String& _value) const;

	int Replace(const char* _pFrom, const String& _to);
	int Replace(const String& _from, const String& _to);

	// offset 인덱스에서 len길이만큼 to문자열로 변경 후 offset + len(이후 위치)을 반환
	// from을 찾지 못했거나 마지막 위치에 도달한 경우 -1을 반환
	int Replace(int _offset, int _length, const String& _to);

	// offset 인덱스부터 from을 찾아서 to로 바꿈
	int Replace(int _offset, const char* _pFrom, const String& _to);	
	int Replace(int _offset, const String& _from, const String& _to);
	void ReplaceAll(const char* _pFrom, const char* _pTo);

	bool Contain(const char* _pStr) const;
	bool Contain(const String& _str) const;
	void Format(const char* _format, ...);
	
	void SetAt(int _idx, char _ch);
	void SetAtForce(int _idx, char _ch);
	char GetAt(int _idx) const ;
	String GetRange(int _startIdx, int _endIdx) const;
	String SubStr(int _startIdx, int _count) const;

	// 동적할당된 문자열, 길이, 할당된 크기를 반환한다.
	Tuple<char*, int, int> GetRangeUnsafe(int _startIdx, int _endIdx) const;
	Vector<String, CDefaultAllocator> Split(const char* _delimiter, bool _includeEmpty = false) const;
	Vector<String, CDefaultAllocator> Split(char _delimiter, bool _includeEmpty = false) const;
	
	void Initialize(int _capacity = DEFAULT_BUFFER_SIZE);

	String ToLowerCase() const;
	String ToUpperCase() const;
	int LeadingZeroCount() const;

	std::string ToStd();

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

public:
	char& operator[](int _idx) const;

	template <typename T>
	String operator+(const T& _other) { 
		String temp = *this;
		temp.Append(_other);
		return temp;
	}

	String operator+(const String& _other) const;
	String operator+(char _ch) const;
	String operator+(const char* _pStr) const;

	template <typename T>
	String& operator+=(const T& _other) { Append(_other);  return *this; }
	String& operator+=(const String& _other);
	String& operator+=(char _ch);
	String& operator+=(char* _pStr);
	String& operator+=(const char* _pStr);

	template <typename T>
	String& operator=(const T& _other) {
		if (pBuffer_ == nullptr)
			Initialize();

		Clear();
		Append(_other);
		return *this;
	}

	String& operator=(const String& _other);
	String& operator=(String&& _other) noexcept;
	String& operator=(const char* _pOther);
	String& operator=(std::nullptr_t _other);
	bool operator==(const String& _other) const;
	bool operator==(const char* _pOther) const;

	bool operator<(const String& _other) const;
	bool operator<(const char* _pOther) const;
	bool operator>(const String& _other) const;
	bool operator>(const char* _pOther) const;
	bool operator<=(const String& _other) const;
	bool operator<=(const char* _pOther) const;
	bool operator>=(const String& _other) const;
	bool operator>=(const char* _pOther) const;

	friend std::ostream& operator<<(std::ostream& _os, const String& _src);
private:
	void ThrowIfInvalidRangeIndex(int _startIdx, int _endIdx) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(int _idx) const;
private:
	char* pBuffer_{};
	int len_{};
	int capacity_{};
	
	friend class StringUtil;
};


NS_END

