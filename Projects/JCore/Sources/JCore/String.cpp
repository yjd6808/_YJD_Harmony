/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/

#include <JCore/Core.h>
#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/Memory.h>
#include <JCore/Exception.h>


namespace JCore {

// 초기 버퍼의 크기
const int String::DEFAULT_BUFFER_SIZE = 32;
// 더해준 문자열과 기존 문자열의 길이를 합쳤을 때 용량을 초과할 경우 배열 크기를 몇배 확장시켜줄지
const float String::EXPANDING_FACTOR = 1.5f;
const char* String::EMPTY = "";

String::String() :
	m_pBuffer(nullptr),
	m_iLen(0),
	m_iCapacity(0) {
	Initialize();
}

String::String(const int capacity) : String(EMPTY, capacity) {
}

String::String(const char* str, const int capacity)
	: m_pBuffer(nullptr) {


	if (str == nullptr) {
		throw NullPointerException("문자열이 nullptr 입니다.");
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
	const int iDstCapacity = int(iDstLen * EXPANDING_FACTOR);

	if (iDstLen >= m_iCapacity) {
		Resize(iDstCapacity);
	}

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
	const int iDstCapacity = int(iDstLen * EXPANDING_FACTOR);

	if (iDstLen >= m_iCapacity) {
		Resize(iDstCapacity);
	}

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

void String::Append(const String&& str) {
	if (str.Length() == 0) {
		return;
	}

	Append(str.m_pBuffer);
}


void String::Resize(const int capacity) {
	char* pTempBuffer = m_pBuffer;

	m_pBuffer = new char[capacity];
	m_iCapacity = capacity;

	StringUtil::Copy(m_pBuffer, m_iCapacity, pTempBuffer);
	DeleteArraySafe(pTempBuffer);
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
		else if (*pSrc < *str)
			return -1;

		pSrc++;
		str++;
	}

	if (m_iLen > iStrLen)
		return 1;
	else if (m_iLen < iStrLen)
		return -1;

	return 0;
}

std::vector<int> String::FindAll(int startIdx, int endIdx, const char* str) const {
	std::vector<int> offsets;
	char* pSrc = m_pBuffer + startIdx;

	const int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = startIdx;

	if (iStrLen == 0) {
		return {};
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	while (iOffset <= endIdx) {
		char* pDst = const_cast<char*>(str);

		while (*pDst != NULL && *pSrc == *pDst) {
			iContinuousCount++;
			pSrc++;
			pDst++;
		}

		if (iContinuousCount == iStrLen && iOffset + iContinuousCount - 1 <= endIdx) {
			offsets.push_back(iOffset);
		}

		if (iContinuousCount == 0) {
			pSrc++;
			iOffset++;
		} else {
			iOffset += iContinuousCount;
		}

		iContinuousCount = 0;
	}

	return offsets;
}

// 문자열을 찾음 : 시작 인덱스 반환
// 문자열을 못찾음 : -1을 반환
std::vector<int> String::FindAll(const char* str) const {
	return FindAll(0, m_iLen - 1, str);
}

std::vector<int> String::FindAll(const String& str) const {
	return FindAll(str.m_pBuffer);
}


// 문자열의 startIdx(시작인덱스 - 포함)부터 endIdx(종료인덱스 - 포함) 포함하여 str문자열이 있을 경우의 위치 인덱스를 반환해줍니다.
// O(n)
int String::Find(int startIdx, int endIdx, const char* str) const {
	char* pSrc = m_pBuffer + startIdx;

	const int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = startIdx;

	if (iStrLen == 0) {
		return -1;
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	while (iOffset <= endIdx) {
		char* pDst = const_cast<char*>(str);

		while (*pDst != NULL && *pSrc == *pDst) {
			iContinuousCount++;
			pSrc++;
			pDst++;
		}

		if (iContinuousCount == iStrLen && iOffset + iContinuousCount - 1 <= endIdx) {
			return iOffset;
		}

		if (iContinuousCount == 0) {
			pSrc++;
			iOffset++;
		} else {
			iOffset += iContinuousCount;
		}

		iContinuousCount = 0;
	}

	return -1;
}

// 문자열을 찾음 : 시작 인덱스 반환
// 문자열을 못찾음 : -1을 반환
int String::Find(const char* str) const {
	return Find(0, m_iLen - 1, str);
}

int String::Find(const String& str) const {
	return Find(str.m_pBuffer);
}

// 문자열의 startIdx(시작인덱스 - 포함)부터 endIdx(종료인덱스 - 포함) 포함하여 str문자열이 있을 경우의 위치 인덱스를 반환해줍니다.
// Find 함수와 결과는 완전히 동일하지만 탐색 방향이 반대입니다.
// O(n)
int String::FindReverse(int startIdx, int endIdx, const char* str) const {
	char* pSrc = m_pBuffer + endIdx;

	const int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = endIdx;

	if (iStrLen == 0) {
		return -1;
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	while (iOffset >= startIdx) {
		char* pDst = const_cast<char*>(str) + iStrLen - 1;

		while (*pDst != NULL && *pSrc == *pDst) {
			iContinuousCount++;
			pSrc--;
			pDst--;
		}

		if (iContinuousCount == iStrLen && iOffset - iContinuousCount + 1 >= startIdx) {
			return iOffset - iContinuousCount + 1;
		}

		if (iContinuousCount == 0) {
			pSrc--;
			iOffset--;
		} else {
			iOffset -= iContinuousCount;
		}

		iContinuousCount = 0;
	}

	return -1;

}

int String::FindReverse(const String& str) const {
	return FindReverse(0, m_iLen - 1, str.m_pBuffer);
}

int String::FindReverse(const char* str) const {
	return FindReverse(0, m_iLen - 1, str);
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

	std::vector<int> iFindOffsets = FindAll(from);
	const int iToLen = StringUtil::Length(to);

	if (iFindOffsets.empty()) {
		return;
	}

	// 길이가 같을 경우 그냥 바꾸면 된다.
	if (iFromLen == iToLen) {
		ReplaceAllWithEqualLen(to, iFromLen, iFindOffsets);
		return;
	}

	ReplaceAllWithDifferentLen(from, to, iFromLen, iToLen);
}

void String::ReplaceAllWithEqualLen(const char* to, const int len, std::vector<int>& offsets) const {
	for (int i = 0; i < offsets.size(); i++) {
		Memory::CopyUnsafe(&m_pBuffer[offsets[i]], to, len);
	}
}

void String::ReplaceAllWithDifferentLen(const char* from, const char* to, const int fromLen, const int toLen) {
	const std::vector<String> vecTokens = Split(from, true);

	const int iExceededSize = toLen * vecTokens.size() - fromLen * vecTokens.size();		// 추가로 확장되어야할 버퍼 크기
	const int iExpectedCapaity = iExceededSize + m_iLen + 1;

	if (iExpectedCapaity > m_iCapacity) {
		Initialize(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
	}

	m_iLen = 0;

	for (size_t i = 0; i < vecTokens.size() - 1; i++) {
		Append(vecTokens[i]);
		Append(to);
	}
	Append(vecTokens[vecTokens.size() - 1]);
}

void String::SetAt(const int idx, const char ch) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	m_pBuffer[idx] = ch;
}

const char String::GetAt(const int idx) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	return m_pBuffer[idx];
}

String String::GetRange(const int startIdx, const int endIdx) const {
	return String(GetRangeUnsafe(startIdx, endIdx));
}

// 기존 문자열의 시작인덱스(포함)부터 종료인덱스(포함)까지의 부분 문자열을 반환합니다.
char* String::GetRangeUnsafe(const int startIdx, const int endIdx) const {
	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("올바르지 않은 인덱스 범위입니다.");
	}

	char* pStr = m_pBuffer + startIdx;
	int iCurIdx = startIdx;
	int iIdx = 0;
	char* szRange = new char[endIdx - startIdx + 10];

	while (iCurIdx <= endIdx) {
		szRange[iIdx] = m_pBuffer[iCurIdx];
		iCurIdx++;
		iIdx++;
	}

	szRange[iIdx] = NULL;
	return szRange;
}

// delimiter 문자열 기준으로 분리합니다.
// includeEmpty가 true일 경우 분리된 토큰 문자열이 비어있더라도 포함 시킵니다.
// O(n)
std::vector<String> String::Split(const char* delimiter, const bool includeEmpty) const {
	std::vector<String> vecTokens;
	int iOffset = Find(delimiter);

	if (iOffset == -1) {
		vecTokens.emplace_back(m_pBuffer);
		return vecTokens;
	}

	const int iDelimiterLen = StringUtil::Length(delimiter);
	if (iOffset - 1 < 0) {
		if (includeEmpty) {
			vecTokens.emplace_back(EMPTY);
		}
	} else {
		vecTokens.emplace_back(GetRangeUnsafe(0, iOffset - 1));
	}

	iOffset += iDelimiterLen;

	while (iOffset < m_iLen) {
		const int iNextOffset = Find(iOffset, m_iLen - 1, delimiter);

		if (iNextOffset == -1) {
			break;
		}
		
		if (iNextOffset <= iOffset) {
			if (includeEmpty) {
				vecTokens.emplace_back(EMPTY);
			}
		} else {
			vecTokens.emplace_back(GetRangeUnsafe(iOffset, iNextOffset - 1));
		}
		iOffset = iNextOffset + 1;
	}

	if (iOffset < m_iLen) {
		vecTokens.emplace_back(GetRangeUnsafe(iOffset, m_iLen - 1));
	} else {
		if (includeEmpty) {
			vecTokens.emplace_back(EMPTY);
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

char& String::operator[](const int idx) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

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


} // namespace JCore


