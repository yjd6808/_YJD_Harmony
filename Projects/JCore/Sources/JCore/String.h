/*
	�ۼ��� : ������
	operator �Լ� �н��� ���� �����غ� ���ڿ� Ŭ�����Դϴ�.
*/

#pragma once

#include <string>
#include <sstream>
#include <ostream>
#include <vector>

namespace JCore {

// variadic template forward declaration
template <typename...>
struct Tuple;

class String final
{
public:
	
	inline static const int DEFAULT_BUFFER_SIZE = 32;	// �ʱ� ������ ũ��
	inline static const int EXPANDING_FACTOR = 2;		// ������ ���ڿ��� ���� ���ڿ��� ���̸� ������ �� �뷮�� �ʰ��� ��� �迭 ũ�⸦ ��� Ȯ���������
	inline static const char* EMPTY = "";

	String();
	String(const int capacity);
	String(const char* str, const int capacity);
	String(const char* str);
	String(std::string& str);
	String(const String& str);
	String(String&& str) noexcept;
	~String();
public:
	char* Source() const { return m_pBuffer; }
	int Capacity() const { return m_iCapacity; }
	int Length() const { return m_iLen; }
	bool Empty() const { return m_iLen == 0; }
	bool IsValidIndex(const int idx) const { return idx >= 0 || idx < m_iLen; }
	bool IsValidIndexRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < m_iLen;
	}
public:
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
	void ResizeIfNeeded(int len);		// len�� m_iCapcity �̻��� ��쿡 x2�ؼ� Ȯ��

	int Compare(const String& str) const;
	int Compare(const char* str, const int strLen = -1) const;
	std::vector<int> FindAll(int startIdx, int endIdx, const char* str) const;
	std::vector<int> FindAll(const char* str) const;
	std::vector<int> FindAll(const String& str) const;
	int Find(int startIdx, int endIdx, const char* str) const;
	int Find(int startIdx, const char* str) const;
	int Find(int startIdx, const String& str) const;
	int Find(const char* str) const;
	int Find(const String& str) const;
	int FindReverse(int startIdx, int endIdx, const char* str) const;
	int FindReverse(const String& str) const;
	int FindReverse(const char* str) const;

	void Clear();
	void Clear(int offset, int len);		// offset �ε������� len��ŭ ����

	int Count(const char* str) const;
	int Count(const String& val) const;
	int Count(const int startIdx, const int endIdx, const char* str) const;
	int Count(const int startIdx, const int endIdx, const String& val) const;

	int Replace(const char* from, const String& to);
	int Replace(const String& from, const String& to);
	int Replace(int offset, int len, const String& to);				// offset �ε������� len���̸�ŭ to���ڿ��� ���� �� offset + len�� ��ȯ

	bool Contain(const char* str) const;
	bool Contain(const String& str) const;
	void Format(const char* format, ...);
	void ReplaceAll(const char* from, const char* to);
	void SetAt(const int idx, const char ch);
	char& GetAt(const int idx);
	String GetRange(const int startIdx, const int endIdx) const;

	// �����Ҵ�� ���ڿ�, ����, �Ҵ�� ũ�⸦ ��ȯ�Ѵ�.
	Tuple<char*, int, int> GetRangeUnsafe(const int startIdx, const int endIdx) const;
	std::vector<String> Split(const char* delimiter, const bool includeEmpty = false) const;
	void Initialize(int capacity = DEFAULT_BUFFER_SIZE);

	String ToLowerCase() const;
	String ToUpperCase() const;
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
	void ReplaceAllWithEqualLen(const char* to, const int fromLen, std::vector<int>& offsets) const;
	void ReplaceAllWithDifferentLen(const char* from, const char* to, const int fromLen, const int toLen);

	void ThrowIfInvalidRangeIndex(const int startIdx, const int endIdx) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(const int idx) const;
private:
	char* m_pBuffer{};
	int m_iLen{};
	int m_iCapacity{};
	
	friend class StringUtil;
};


} // namespace JCore

