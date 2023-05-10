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
class DefaultAllocator;

template <typename, typename>
class Vector;

class String final
{
public:
	
	inline static const int DefaultBufferSize = 32;	// 초기 버퍼의 크기
	inline static const int ExpandingFactor = 2;		// 더해준 문자열과 기존 문자열의 길이를 합쳤을 때 용량을 초과할 경우 배열 크기를 몇배 확장시켜줄지
	inline static const char* EmptyString = "";

	String();
	String(const int capacity);
	String(const char* str, const int capacity);
	String(const char* str);
	String(char ch, int count);
	String(const std::string& str);
	String(const String& str);
	String(String&& str) noexcept;
	~String();
public:
	char* Source() const { return m_pBuffer; }
	int Capacity() const { return m_iCapacity; }
	int Length() const { return m_iLen; }
	int LengthWithNull() const { return m_iLen + 1; }
	void SetLength(int len) { m_iLen = len; }
	void ExchangeSource(char* src, int len);
	bool Empty() const { return m_iLen == 0; }
    bool IsNull() const { return m_pBuffer == nullptr; }
	bool IsValidIndex(const int idx) const { return idx >= 0 || idx < m_iLen; }
	bool IsValidIndexRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < m_iLen;
	}

	template <typename T>
	void Append(const T& str) {
		std::stringstream ss;
		ss << str;
		Append(ss.str());
	}
	void Append(const char ch);
	void Append(const char* str);
	void Append(const std::string& str);
	void Append(const String& str);
	void Append(String&& str);

	void Insert(const int idx, const char* str);
	void Insert(const int idx, const String& str);

	void Resize(const int capacity);
	void ResizeIfNeeded(int len);		// len이 m_iCapcity 이상일 경우에 x2해서 확장

	int Compare(const String& str) const;
	int Compare(const char* str, const int strLen = -1) const;
	Vector<int, DefaultAllocator> FindAll(int startIdx, int endIdx, const char* str) const;
	Vector<int, DefaultAllocator> FindAll(const char* str) const;
	Vector<int, DefaultAllocator> FindAll(const String& str) const;
	int Find(int startIdx, int endIdx, const char* str) const;
	int Find(int startIdx, const char* str) const;
	int Find(int startIdx, const String& str) const;
	int Find(const char* str) const;
	int Find(const String& str) const;
	int FindReverse(int startIdx, int endIdx, const char* str) const;
	int FindReverse(const String& str) const;
	int FindReverse(const char* str) const;

	bool EndWith(const String& str) { return FindReverse(str.Source()) == m_iLen - str.Length();  }
	bool StartWith(const String& str) { return Find(str) == 0; }

	char Last() const { return GetAt(m_iLen - 1); }
	char First() const { return GetAt(0); }

	void Clear();
	void Clear(int offset, int len);		// offset 인덱스에서 len만큼 없앰

	int Count(const char* str) const;
	int Count(const String& val) const;
	int Count(const int startIdx, const int endIdx, const char* str) const;
	int Count(const int startIdx, const int endIdx, const String& val) const;

	int Replace(const char* from, const String& to);
	int Replace(const String& from, const String& to);

	// offset 인덱스에서 len길이만큼 to문자열로 변경 후 offset + len(이후 위치)을 반환
	// from을 찾지 못했거나 마지막 위치에 도달한 경우 -1을 반환
	int Replace(int offset, int len, const String& to);

	// offset 인덱스부터 from을 찾아서 to로 바꿈
	int Replace(int offset, const char* from, const String& to);	
	int Replace(int offset, const String& from, const String& to);
	void ReplaceAll(const char* from, const char* to);

	bool Contain(const char* str) const;
	bool Contain(const String& str) const;
	void Format(const char* format, ...);
	
	void SetAt(const int idx, const char ch);
	char GetAt(const int idx) const ;
	String GetRange(const int startIdx, const int endIdx) const;

	// 동적할당된 문자열, 길이, 할당된 크기를 반환한다.
	Tuple<char*, int, int> GetRangeUnsafe(const int startIdx, const int endIdx) const;
	Vector<String, DefaultAllocator> Split(const char* delimiter, const bool includeEmpty = false) const;
	void Initialize(int capacity = DefaultBufferSize);

	String ToLowerCase() const;
	String ToUpperCase() const;

	std::string ToStd();
public:
	char& operator[](const int idx) const;

	template <typename T>
	String operator+(const T& other) { 
		String temp = *this;
		temp.Append(other);
		return temp;
	}

	String operator+(const String& other) const;
	String operator+(const char ch) const;
	String operator+(const char* str) const;

	template <typename T>
	String& operator+=(const T& other) { Append(other);  return *this; }
	String& operator+=(const String& other);
	String& operator+=(const char ch);
	String& operator+=(const char* str);

	template <typename T>
	String& operator=(const T& other) {
		if (m_pBuffer == nullptr)
			Initialize();

		Clear();
		Append(other);
		return *this;
	}

	String& operator=(const String& other);
	String& operator=(String&& other) noexcept;
	String& operator=(const char* other);
	bool operator==(const String& other) const;
	bool operator==(const char* other) const;
	bool operator!=(const String& other) const;
	bool operator!=(const char* other) const;

	bool operator<(const String& other) const;
	bool operator<(const char* other) const;
	bool operator>(const String& other) const;
	bool operator>(const char* other) const;
	bool operator<=(const String& other) const;
	bool operator<=(const char* other) const;
	bool operator>=(const String& other) const;
	bool operator>=(const char* other) const;

	friend std::ostream& operator<<(std::ostream& os, const String& src);
private:
	void ThrowIfInvalidRangeIndex(const int startIdx, const int endIdx) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(const int idx) const;
private:
	char* m_pBuffer{};
	int m_iLen{};
	int m_iCapacity{};
	
	friend class StringUtil;
};


NS_JC_END

