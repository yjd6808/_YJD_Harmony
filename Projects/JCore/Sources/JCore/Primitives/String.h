/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/

#pragma once

#include <string>
#include <sstream>
#include <ostream>

#include <JCore/Namespace.h>

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
	static String Empty;
	static String Null;

	String();
	String(int _capacity);
	String(const char* _pStr, int _capacity);
	String(const char* _pStr);
	String(char _ch, int _count);
	String(const std::string& _str);
	String(const String& _str);
	String(String&& _str) noexcept;
	~String();
public:
	char* Source() const { return m_pBuffer; }
	const char* SafeSource() const { return m_pBuffer != nullptr ? m_pBuffer : EmptySource; }
	int Capacity() const { return m_iCapacity; }
	int Length() const { return m_iLen; }
	int LengthWithNull() const { return m_iLen + 1; }
	void SetLength(int _length) { m_iLen = _length; }
	void ExchangeSource(char* _pSrc, int _len);
	bool IsEmpty() const { return m_iLen == 0; }
    bool IsNull() const { return m_pBuffer == nullptr; }
	bool IsValidIndex(const int _index) const { return _index >= 0 || _index < m_iLen; }
	bool IsValidIndexRange(const int _startIndex, const int _endIndex) const 
	{
		return _startIndex <= _endIndex && _startIndex >= 0 && _endIndex < m_iLen;
	}

	template <typename T>
	void Append(const T& _str) 
	{
		std::stringstream ss;
		ss << _str;
		Append(ss.str());
	}
	void Append(char _ch);
	void Append(char* _pStr);
	void Append(const char* _pStr);
	void Append(const std::string& _str);
	void Append(const String& _str);
	void Append(String&& _str);

	void Insert(int _index, const char* _pStr);
	void Insert(int _index, const String& _str);

	void Resize(int _capacity);
	void ResizeIfNeeded(int _length);		// len이 m_iCapcity 이상일 경우에 x2해서 확장

	int Compare(const String& _str) const;
	int Compare(const char* _pStr, int _strLen = -1) const;
	Vector<int, CDefaultAllocator> FindAll(int _startIndex, int _endIndex, const char* _pStr) const;
	Vector<int, CDefaultAllocator> FindAll(const char* _pStr) const;
	Vector<int, CDefaultAllocator> FindAll(const String& _str) const;
	int Find(int _startIndex, int _endIndex, const char* _pStr) const;
	int Find(int _startIndex, const char* _pStr) const;
	int Find(int _startIndex, const String& _str) const;
	int Find(const char* _pStr) const;
	int Find(const String& _str) const;
	int FindReverse(int _startIndex, int _endIndex, const char* _pStr) const;
	int FindReverse(const String& _str) const;
	int FindReverse(const char* _pStr) const;

	bool EndWith(const String& _str) const { return FindReverse(_str.Source()) == m_iLen - _str.Length();  }
	bool StartWith(const String& _str) const { return Find(_str) == 0; }

	char Last() const { return GetAt(m_iLen - 1); }
	char First() const { return GetAt(0); }

	void Clear();
	void Clear(int _offset, int _length);		// offset 인덱스에서 len만큼 없앰

	int Count(const char* _pStr) const;
	int Count(const String& _value) const;
	int Count(int _startIndex, int _endIndex, const char* _pStr) const;
	int Count(int _startIndex, int _endIndex, const String& _value) const;

	int Replace(const char* _pFrom, const String& _to);
	int Replace(const String& _from, const String& _to);

	// offset 인덱스에서 len길이만큼 to문자열로 변경 후 offset + len(이후 위치)을 반환
	// from을 찾지 못했거나 마지막 위치에 도달한 경우 -1을 반환
	int Replace(int _offset, int _length, const String& _to);

	// offset 인덱스부터 from을 찾아서 to로 바꿈
	int Replace(int _offset, const char* _pFrom, const String& _to);	
	int Replace(int _offset, const String& _from, const String& _to);
	void ReplaceAll(const char* _pFrom, const char* _pTo);

	bool Contain(const char* str) const;
	bool Contain(const String& str) const;
	void Format(const char* format, ...);
	
	void SetAt(int idx, char ch);
	char GetAt(int idx) const ;
	String GetRange(int startIdx, int endIdx) const;

	// 동적할당된 문자열, 길이, 할당된 크기를 반환한다.
	Tuple<char*, int, int> GetRangeUnsafe(int startIdx, int endIdx) const;
	Vector<String, CDefaultAllocator> Split(const char* delimiter, bool includeEmpty = false) const;
	void Initialize(int capacity = DEFAULT_BUFFER_SIZE);

	String ToLowerCase() const;
	String ToUpperCase() const;

	int LeadingZeroCount() const;

	std::string ToStd();
public:
	char& operator[](int _index) const;

	template <typename T>
	String operator+(const T& other) { 
		String temp = *this;
		temp.Append(other);
		return temp;
	}

	String operator+(const String& _other) const;
	String operator+(char _ch) const;
	String operator+(const char* _pStr) const;

	template <typename T>
	String& operator+=(const T& other) { Append(other);  return *this; }
	String& operator+=(const String& _other);
	String& operator+=(char _ch);
	String& operator+=(char* _pStr);
	String& operator+=(const char* _pStr);

	template <typename T>
	String& operator=(const T& other) {
		if (m_pBuffer == nullptr)
			Initialize();

		Clear();
		Append(other);
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
	void ThrowIfInvalidRangeIndex(int _startIndex, int _endIndex) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(int _index) const;
private:
	char* m_pBuffer{};
	int m_iLen{};
	int m_iCapacity{};
	
	friend class StringUtil;
};


NS_JC_END

