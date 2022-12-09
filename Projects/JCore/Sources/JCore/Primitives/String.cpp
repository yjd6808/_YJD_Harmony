/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/


#include <JCore/Core.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Memory.h>
#include <JCore/Exception.h>
#include <JCore/Ascii.h>
#include <JCore/Tuple.h>
#include <JCore/Container/Vector.h>

namespace JCore {

String::String() {
	Initialize();
}

String::String(const int capacity)  {
	if (capacity == 0) {
		m_pBuffer = nullptr;
		m_iCapacity = 0;
		m_iLen = 0;
	} else {
		*this = String(EMPTY, capacity);
	}
}

String::String(const char* str, const int capacity) {
	if (str == nullptr) {
        m_pBuffer = nullptr;
        m_iCapacity = 0;
        m_iLen = 0;
        return;
	}

	const int iLen = StringUtil::Length(str);
	int iExpectedCapcity = int(iLen * EXPANDING_FACTOR);

	if (iExpectedCapcity < capacity) {
		iExpectedCapcity = capacity;
	}

	if (iLen == 0) {
		Initialize(capacity);
		return;
	}

	m_pBuffer = new char[iExpectedCapcity];
	m_iCapacity = iExpectedCapcity;
	m_iLen = iLen;

	StringUtil::Copy(m_pBuffer, m_iCapacity, str);
}

String::String(const char* str) : String(str, DEFAULT_BUFFER_SIZE) {
}

String::String(std::string& str) : String(str.c_str()) {
}

String::String(const String& str) : String(str.m_pBuffer) {
	
}


String::String(String&& str) noexcept {
	// 이동 대입 연산자 호출
	*this = std::move(str);
}

String::~String() {
	DeleteArraySafe(m_pBuffer);
}

/* ========================================================== */

void String::Append(const char ch) {
	const int iDstLen = m_iLen + 1;
	ResizeIfNeeded(iDstLen);

	Memory::CopyUnsafe(m_pBuffer + m_iLen, &ch, 1);
	m_pBuffer[iDstLen] = NULL;
	m_iLen = iDstLen;
}

void String::Append(const char* str) {
	if (str == nullptr) {
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	const int iStrLen = StringUtil::Length(str);
	const int iDstLen = m_iLen + iStrLen;
	ResizeIfNeeded(iDstLen);

	Memory::CopyUnsafe(m_pBuffer + m_iLen, str, iStrLen);
	m_pBuffer[iDstLen] = NULL;
	m_iLen = iDstLen;
}


void String::Append(const std::string& str) {
	if (str.empty()) {
		return;
	}

	Append(str.c_str());
}

void String::Append(const String& str) {
	if (str.Length() == 0) {
		return;
	}

	Append(str.m_pBuffer);
}

void String::Append(String&& str) {
	if (str.Length() == 0) {
		return;
	}

	Append(str.m_pBuffer);
}

void String::Insert(const int idx, const char* str) {
	const int iLen = StringUtil::Length(str);

	ThrowIfInvalidIndex(idx);
	ResizeIfNeeded(m_iLen + iLen);

	// 문자열 길이만큼 뒤로 밀어줌
	// 기존문자열이 완전히 비어있거나, 맨뒤에 삽입하는 경우
	// m_iLen - idx가 0이 되버려서 아예 복사핧 데이터가 없는 경우가 있을 수 있다.
	if (m_iLen - idx > 0) {
		Memory::CopyReverse(
			m_pBuffer + idx + iLen,
			m_iCapacity - idx - iLen,
			m_pBuffer + idx,
			m_iLen - idx);
	}

	for (int i = 0; i < iLen; ++i) {
		m_pBuffer[i + idx] = str[i];
	}

	m_iLen += iLen;
	m_pBuffer[m_iLen] = NULL;
}

void String::Insert(const int idx, const String& str) {
	Insert(idx, str.Source());
}


void String::Resize(const int capacity) {
	char* pTempBuffer = m_pBuffer;

	m_pBuffer = new char[capacity];
	m_iCapacity = capacity;

	StringUtil::Copy(m_pBuffer, m_iCapacity, pTempBuffer);
	DeleteArraySafe(pTempBuffer);
}

// len길이가 현재 capacity를 초과할 경우 확장을 해준다.
void String::ResizeIfNeeded(int len) {
	bool bNeedResize = false;
	if (len >= m_iCapacity) {
		len *= EXPANDING_FACTOR;
		bNeedResize = true;
	}

	if (bNeedResize)
		Resize(len + 1);
}


int String::Compare(const String& str) const {
	return Compare(str.Source());
}

// 0  : 같음
// 1  : 기존 문자열(this)이 우선순위가 더 큼
// -1 : 비교 문자열(str)이 우선순위가 더 큼
// O(n)
int String::Compare(const char* str, const int strLen) const {
	const int iStrLen = strLen == -1 ? StringUtil::Length(str) : strLen;
	char* pSrc = m_pBuffer;

	while (*pSrc != NULL && *str != NULL) {
		if (*pSrc > *str)
			return 1;
		if (*pSrc < *str)
			return -1;

		pSrc++;
		str++;
	}

	if (m_iLen > iStrLen)
		return 1;
	if (m_iLen < iStrLen)
		return -1;

	return 0;
}

Vector<int> String::FindAll(int startIdx, int endIdx, const char* str) const {

	Vector<int> offsets;
	const int iStrLen = StringUtil::Length(str);

	if (iStrLen == 0) {
		return offsets;
	}

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	for (int i = startIdx; i <= endIdx; ) {
		int iFind = Find(i, endIdx, str);

		if (iFind == -1) {
			break;
		}

		offsets.PushBack(iFind);
		i += iFind + iStrLen;
	}

	
	return offsets;
}

// 문자열을 찾음 : 시작 인덱스 반환
// 문자열을 못찾음 : -1을 반환
Vector<int> String::FindAll(const char* str) const {
	return FindAll(0, m_iLen - 1, str);
}

Vector<int> String::FindAll(const String& str) const {
	return FindAll(str.m_pBuffer);
}


// 문자열의 startIdx(시작인덱스 - 포함)부터 endIdx(종료인덱스 - 포함) 포함하여 str문자열이 있을 경우의 위치 인덱스를 반환해줍니다.
// O(n)
int String::Find(int startIdx, int endIdx, const char* str) const {
	const int iFindStrLen = StringUtil::Length(str);
	const int iSrcLen = endIdx - startIdx + 1;

	if (iFindStrLen == 0) {
		return 0;
	}

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	if (iFindStrLen > iSrcLen) {
		return -1;
	}

	for (int i = startIdx; i <= endIdx; i++) {
		int iContinuous = 0;

		while (iContinuous < iFindStrLen && m_pBuffer[i + iContinuous] == str[iContinuous]) {
			iContinuous++;
		}

		if (iContinuous == iFindStrLen) {
			return i;
		}
	}

	return -1;
}

int String::Find(int startIdx, const char* str) const {
	return Find(startIdx, m_iLen - 1, str);
}

// 문자열을 찾음 : 시작 인덱스 반환
// 문자열을 못찾음 : -1을 반환
int String::Find(const char* str) const {
	return Find(0, m_iLen - 1, str);
}

int String::Find(const String& str) const {
	return Find(str.m_pBuffer);
}

int String::Find(int startIdx, const String& str) const {
	return Find(startIdx, m_iLen - 1, str.Source());
}

// 문자열의 startIdx(시작인덱스 - 포함)부터 endIdx(종료인덱스 - 포함) 포함하여 str문자열이 있을 경우의 위치 인덱스를 반환해줍니다.
// Find 함수와 결과는 완전히 동일하지만 탐색 방향이 반대입니다.
// O(n)
int String::FindReverse(int startIdx, int endIdx, const char* str) const {
	const int iFindStrLen = StringUtil::Length(str);
	const int iSrcLen = endIdx - startIdx + 1;

	if (iFindStrLen == 0) {
		return 0;
	}

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	if (iFindStrLen > iSrcLen) {
		return -1;
	}

	for (int i = endIdx; i >= startIdx; i--) {
		int iContinuous = 0;

		while (iContinuous < iFindStrLen && m_pBuffer[i + iContinuous] == str[iContinuous]) {
			iContinuous++;
		}

		if (iContinuous == iFindStrLen) {
			return i;
		}
	}

	return -1;

}

int String::FindReverse(const String& str) const {
	return FindReverse(0, m_iLen - 1, str.m_pBuffer);
}

int String::FindReverse(const char* str) const {
	return FindReverse(0, m_iLen - 1, str);
}

void String::Clear() {
	ThrowIfNotInitialized();
	m_iLen = 0;
	m_pBuffer[m_iLen] = NULL;
}

void String::Clear(int offset, int len) {
	ThrowIfInvalidIndex(offset);

	const int iRemoveLen = offset + len > m_iLen ? m_iLen - offset : len;	// 예를들어 abcdefg에서 offset 6에 len 100뭐 이렇게 넣으면 len이 1이 되도록 해줘야함
	const int iMoveCharCount = m_iLen - offset - iRemoveLen;

	Memory::CopyUnsafe(
		m_pBuffer + offset, 
		m_pBuffer + offset + len, 
		iMoveCharCount);

	m_iLen -= iRemoveLen;
	m_pBuffer[m_iLen] = NULL;
}

int String::Count(const char* str) const {
	return Count(0, m_iLen - 1, str);
}

int String::Count(const String& val) const {
	return Count(0, m_iLen - 1, val.Source());
}

/**
 * \brief  startIdx(포함) ~ endIdx (endIdx)사이의 str문자열 갯수를 반환해버렷!
 * O(n)
 */
int String::Count(const int startIdx, const int endIdx, const char* str) const {
	ThrowIfNotInitialized();
	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	const int iStrLen = StringUtil::Length(str);

	int iOffset = 0;
	int iCount = 0;

	while (iOffset <= endIdx && (iOffset = Find(iOffset, endIdx, str)) != -1) {
		iCount++;
		iOffset += iStrLen;
	}
	return iCount;
}

int String::Count(const int startIdx, const int endIdx, const String& val) const {
	return Count(startIdx, endIdx, val.Source());
}

int String::Replace(const char* from, const String& to) {
	return Replace(Find(from), StringUtil::Length(from), to);
}

int String::Replace(const String& from, const String& to) {
	return Replace(Find(from.Source()), from.Length(), to);
}

int String::Replace(int offset, int len, const String& to) {
	if (offset == -1)
		return -1;

	ThrowIfInvalidIndex(offset);
	const int iLen = offset + len > m_iLen ? m_iLen - offset : len;	// 예를들어 abcdefg에서 offset 6에 len 100뭐 이렇게 넣으면 len이 1이 되도록 해줘야함

	
	if (iLen < to.Length()) {
		// 치환하고자하는 문자열의 길이가 더 긴 경우 긴 만큼만 뒤로 밀어준 다음 사이에 하나하나 복사해준다.
		ResizeIfNeeded(m_iLen + to.Length() - iLen);	// 확장해줘야할 수 있음

		Memory::CopyUnsafeReverse(
			m_pBuffer + offset + to.Length(),
			m_pBuffer + offset + iLen,
			m_iLen - offset - iLen
		);

		for (int i = 0; i < to.Length(); ++i) {
			m_pBuffer[offset + i] = to[i];
		}

		m_iLen += to.Length() - iLen;
	} else {
		// 치환하고자하는 문자열의 길이가 같거나 짧은 경우 
		Memory::CopyUnsafe(
			m_pBuffer + offset + to.Length(),
			m_pBuffer + offset + iLen,
			m_iLen - offset - to.Length()
		);

		for (int i = 0; i < to.Length(); ++i) {
			m_pBuffer[offset + i] = to[i];
		}

		m_iLen -= iLen - to.Length();
	}

	m_pBuffer[m_iLen] = NULL;
	const int iNextOffset = offset + to.Length();
	return iNextOffset >= m_iLen ? -1 : iNextOffset;	// 다음 포지션 반환
}

int String::Replace(int offset, const char* from, const String& to) {
	return Replace(Find(offset, from), StringUtil::Length(from), to);
}

int String::Replace(int offset, const String& from, const String& to) {
	return Replace(Find(offset, from), from.Length(), to);
}

bool String::Contain(const char* str) const {
	return Find(str) != -1;
}

bool String::Contain(const String& str) const {
	return Find(str.m_pBuffer) != -1;
}


// @참고 : https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
void String::Format(const char* format, ...) {
	va_list args;
	va_start(args, format);

	const int iExpectedLen = vsnprintf(nullptr, 0, format, args); // 포맷 변환시 필요한 문자열 길이를 획득

	if (iExpectedLen <= 0) {
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	if (m_iCapacity < iExpectedLen + 1) {
		Resize(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	vsnprintf(m_pBuffer, m_iCapacity, format, args);
	m_pBuffer[iExpectedLen] = NULL;
	m_iLen = iExpectedLen;
	
	va_end(args);
}


// 문자열 버퍼(m_pBuffer)에서 from 문자열을 검색하여 to 문자열로 변환합니다.
// O(n)
void String::ReplaceAll(const char* from, const char* to) {
	const int iFromLen = StringUtil::Length(from);
	
	if (iFromLen == 0) {
		*this = to;
		return;
	}

	int iReplaceOffset = 0;
	while ((iReplaceOffset = Replace(iReplaceOffset, from, to)) != -1) {
		// EMPTY
	}
}

void String::SetAt(const int idx, const char ch) {
	ThrowIfInvalidIndex(idx);
	m_pBuffer[idx] = ch;
}

char& String::GetAt(const int idx) {
	ThrowIfInvalidIndex(idx);
	return m_pBuffer[idx];
}

String String::GetRange(const int startIdx, const int endIdx) const {
	String dummy(0);

	auto [ pBuffer, iLen, iCapacity ] = GetRangeUnsafe(startIdx, endIdx);

	dummy.m_pBuffer = pBuffer;
	dummy.m_iLen = iLen;
	dummy.m_iCapacity = iCapacity;

	return dummy;
}

// 기존 문자열의 시작인덱스(포함)부터 종료인덱스(포함)까지의 부분 문자열을 반환합니다.
Tuple<char*, int, int> String::GetRangeUnsafe(const int startIdx, const int endIdx) const {
	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	char* pStr = m_pBuffer + startIdx;
	int iCurIdx = startIdx;
	int iIdx = 0;
	const int iAllocCapacity = endIdx - startIdx + 10;
	char* szRange = new char[iAllocCapacity];

	while (iCurIdx <= endIdx) {
		szRange[iIdx] = m_pBuffer[iCurIdx];
		iCurIdx++;
		iIdx++;
	}

	szRange[iIdx] = NULL;
	return { szRange, iIdx, iAllocCapacity };
}

// delimiter 문자열 기준으로 분리합니다.
// includeEmpty가 true일 경우 분리된 토큰 문자열이 비어있더라도 포함 시킵니다.
// O(n)
Vector<String> String::Split(const char* delimiter, const bool includeEmpty) const {
	Vector<String> vecTokens;
	int iOffset = Find(delimiter);

	if (iOffset == -1) {
		vecTokens.EmplaceBack(m_pBuffer);
		return vecTokens;
	}

	const int iDelimiterLen = StringUtil::Length(delimiter);
	if (iOffset - 1 < 0) {
		if (includeEmpty) {
			vecTokens.EmplaceBack(EMPTY);
		}
	} else {
		vecTokens.EmplaceBack(GetRange(0, iOffset - 1));
	}

	iOffset += iDelimiterLen;

	while (iOffset < m_iLen) {
		const int iNextOffset = Find(iOffset, m_iLen - 1, delimiter);

		if (iNextOffset == -1) {
			break;
		}
		
		if (iNextOffset <= iOffset) {
			if (includeEmpty) {
				vecTokens.EmplaceBack(EMPTY);
			}
		} else {
			vecTokens.EmplaceBack(GetRange(iOffset, iNextOffset - 1));
		}
		iOffset = iNextOffset + 1;
	}

	if (iOffset < m_iLen) {
		vecTokens.EmplaceBack(GetRange(iOffset, m_iLen - 1));
	} else {
		if (includeEmpty) {
			vecTokens.EmplaceBack(EMPTY);
		}
	}

	return vecTokens;
}

void String::Initialize(int capacity) {
	DeleteArraySafe(m_pBuffer);

	m_pBuffer = new char[capacity];
	m_iLen = 0;
	m_iCapacity = capacity;
	m_pBuffer[0] = NULL;
}


String String::ToLowerCase() const {
	String copy = *this;

	for (int i = 0; i < copy.Length(); i++) {
		if (IsUpperCaseAlphabat(copy[i])) {
			copy[i] -= static_cast<char>(32);
		}
	}

	return copy;
}

String String::ToUpperCase() const {
	String copy = *this;

	for (int i = 0; i < copy.Length(); i++) {
		if (IsLowerCaseAlphabat(copy[i])) {
			copy[i] += static_cast<char>(32);
		}
	}

	return copy;
}

char& String::operator[](const int idx) const {
	ThrowIfInvalidIndex(idx);
	return m_pBuffer[idx];
}

String String::operator+(const String& other) const {
	String temp = *this;
	temp.Append(other);
	return temp;
}

String String::operator+(const char ch) const {
	String temp = *this;
	temp.Append(ch);
	return temp;
}

String String::operator+(const char* str) const {
	String temp = *this;
	temp.Append(str);
	return temp;
}

String& String::operator+=(const String& other) {
	Append(other);
	return *this;
}

String& String::operator+=(const char ch) {
	Append(ch);
	return *this;
}

String& String::operator+=(const char* str) {
	Append(str);
	return *this;
}

String& String::operator=(const String& other) {
	return *this = other.Source();
}

// 씹어먹는 C++ 이동 생성자 & 이동 대입 연산자
// @참고 : https://modoocode.com/227
String& String::operator=(String&& other) noexcept {
	// 기존 문자열 제거해줘야함
	DeleteArraySafe(m_pBuffer);

	// 이동 생성자 학습 후 적용
	m_iCapacity = other.m_iCapacity;
	m_iLen = other.m_iLen;
	m_pBuffer = other.m_pBuffer;

	other.m_pBuffer = nullptr;
	return *this;
}

String& String::operator=(const char* other) {
	const int iToLen = StringUtil::Length(other);
	const int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > m_iCapacity) {
		Initialize(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
	}

	StringUtil::Copy(m_pBuffer, m_iCapacity, other);
	m_iLen = iToLen;

	return *this;
}

bool String::operator==(const String& other) const {
	return Compare(other) == 0;
}

bool String::operator==(const char* other) const {
	return Compare(other) == 0;
}

bool String::operator!=(const String& other) const {
	return Compare(other) != 0;
}

bool String::operator!=(const char* other) const {
	return Compare(other) != 0;
}

bool String::operator<(const String& other) const {
	return Compare(other) < 0;
}

bool String::operator<(const char* other) const {
	return Compare(other) < 0;
}

bool String::operator>(const String& other) const {
	return Compare(other) > 0;
}

bool String::operator>(const char* other) const {
	return Compare(other) > 0;
}

bool String::operator<=(const String& other) const {
	return Compare(other) <= 0;
}

bool String::operator<=(const char* other) const {
	return Compare(other) <= 0;
}

bool String::operator>=(const String& other) const {
	return Compare(other) >= 0;
}

bool String::operator>=(const char* other) const {
	return Compare(other) >= 0;
}

std::ostream& operator<<(std::ostream& os, const String& src) {
	os << src.m_pBuffer;
	return os;
}

void String::ThrowIfInvalidRangeIndex(const int startIdx, const int endIdx) const {
	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}
}

void String::ThrowIfNotInitialized() const {
	if (m_pBuffer == nullptr) {
		throw NullPointerException("String을 먼저 초기화해주세요.");
	}
}

void String::ThrowIfInvalidIndex(const int idx) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}
}


} // namespace JCore


