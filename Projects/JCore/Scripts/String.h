/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/

#pragma once

#include <string>
#include <ostream>
#include <vector>

namespace JCore {

class String final
{
public:
	static const int DEFAULT_BUFFER_SIZE;
	static const float EXPANDING_FACTOR;
	static const char* EMPTY;

	String();
	String(const int capacity);
	String(const char* str, const int capacity);
	String(const char* str);
	String(std::string& str);
	String(const String& str);
	String(String&& str) noexcept;
	~String();
public:
	inline char* Source() const { return m_pBuffer; }
	inline const int Capacity() const { return m_iCapacity; }
	inline const int Length() const { return m_iLen; }
	inline const int Empty() const { m_iLen == 0; }
	inline bool IsValidIndex(const int idx) const { return idx >= 0 || idx < m_iLen; }
	inline bool IsValidIndexRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < m_iLen;
	}
public:
	void Append(const char* str);
	void Append(const std::string& str);
	void Append(const String& str);
	void Append(const String&& str);

	void Resize(const int capacity);
	int Compare(const String& str) const;
	int Compare(const char* str, const int strLen = -1) const;
	int Find(int startIdx, int endIdx, const char* str) const;
	int Find(const char* str) const;
	int Find(const String& str) const;
	bool Contain(const char* str) const;
	bool Contain(const String& str) const;
	void Format(const char* format, ...);
	void Replace(const char* from, const char* to);
	void SetAt(const int idx, const char ch);
	const char GetAt(const int idx) const;
	String GetRange(const int startIdx, const int endIdx) const;
	char* GetRangeUnsafe(const int startIdx, const int endIdx) const;
	std::vector<String> Split(const char* delimiter, const bool includeEmpty = false) const;
	void Initialize(int capacity = DEFAULT_BUFFER_SIZE);
public:
	String operator+(const String& other);
	String& operator+=(const String& other);
	String& operator=(const String& other);
	String& operator=(String&& other) noexcept;
	String& operator=(const char* other);
	bool operator==(const String& other);
	bool operator==(const char* other);
	bool operator!=(const String& other);
	bool operator!=(const char* other);

	friend std::ostream& operator<<(std::ostream& os, const String& src);
private:
	char* m_pBuffer;
	int m_iLen;
	int m_iCapacity;
};

} // namespace JCore

