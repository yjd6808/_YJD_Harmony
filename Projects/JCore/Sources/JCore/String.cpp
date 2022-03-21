/*
	�ۼ��� : ������
	operator �Լ� �н��� ���� �����غ� ���ڿ� Ŭ�����Դϴ�.
*/

#include <JCore/Core.h>
#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/Memory.h>
#include <JCore/Exception.h>


namespace JCore {

// �ʱ� ������ ũ��
const int String::DEFAULT_BUFFER_SIZE = 32;
// ������ ���ڿ��� ���� ���ڿ��� ���̸� ������ �� �뷮�� �ʰ��� ��� �迭 ũ�⸦ ��� Ȯ���������
const float String::EXPANDING_FACTOR = 1.5f;
const char* String::EMPTY = "";

String::String() {
	m_pBuffer = nullptr;
	m_iLen = 0;
	m_iCapacity = 0;

	Initialize();
}

String::String(const int capacity) : String(EMPTY, capacity) {
}

String::String(const char* str, const int capacity) {
	m_pBuffer = nullptr;

	if (str == nullptr) {
		throw NullPointerException("���ڿ��� nullptr �Դϴ�.");
	}

	int iLen = StringUtil::Length(str);
	int iExpectedCapcity = int(iLen * EXPANDING_FACTOR);

	if (iExpectedCapcity < capacity) {
		iExpectedCapcity = capacity;
	}

	if (iLen == 0) {
		Initialize();
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
	// �̵� ���� ������ ȣ��
	*this = std::move(str);
}

String::~String() {
	DeleteArraySafe(m_pBuffer);
}

/* ========================================================== */

void String::Append(const char ch) {
	int iDstLen = m_iLen + 1;
	int iDstCapacity = int(iDstLen * EXPANDING_FACTOR);

	if (iDstLen >= m_iCapacity) {
		Resize(iDstCapacity);
	}

	Memory::CopyUnsafe(m_pBuffer + m_iLen, &ch, 1);
	m_pBuffer[iDstLen] = NULL;
	m_iLen = iDstLen;
}

void String::Append(const char* str) {
	if (str == nullptr) {
		throw NullPointerException("�߰��ϰ��� �ϴ� ���ڿ��� nullptr �Դϴ�.");
	}

	int iStrLen = StringUtil::Length(str);
	int iDstLen = m_iLen + iStrLen;
	int iDstCapacity = int(iDstLen * EXPANDING_FACTOR);

	if (iDstLen >= m_iCapacity) {
		Resize(iDstCapacity);
	}

	Memory::CopyUnsafe(m_pBuffer + m_iLen, str, iStrLen);
	m_pBuffer[iDstLen] = NULL;
	m_iLen = iDstLen;
}


void String::Append(const std::string& str) {
	if (str.size() == 0) {
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

// 0  : ����
// 1  : ���� ���ڿ�(this)�� �켱������ �� ŭ
// -1 : �� ���ڿ�(str)�� �켱������ �� ŭ
// O(n)
int String::Compare(const char* str, const int strLen) const {
	int iStrLen = strLen == -1 ? StringUtil::Length(str) : strLen;
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

	int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = startIdx;

	if (iStrLen == 0) {
		return {};
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("�ε��� ������ ������ϴ�.");
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

// ���ڿ��� ã�� : ���� �ε��� ��ȯ
// ���ڿ��� ��ã�� : -1�� ��ȯ
std::vector<int> String::FindAll(const char* str) const {
	return FindAll(0, m_iLen - 1, str);
}

std::vector<int> String::FindAll(const String& str) const {
	return FindAll(str.m_pBuffer);
}


// ���ڿ��� startIdx(�����ε��� - ����)���� endIdx(�����ε��� - ����) �����Ͽ� str���ڿ��� ���� ����� ��ġ �ε����� ��ȯ���ݴϴ�.
// O(n)
int String::Find(int startIdx, int endIdx, const char* str) const {
	char* pSrc = m_pBuffer + startIdx;

	int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = startIdx;

	if (iStrLen == 0) {
		return -1;
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("�ε��� ������ ������ϴ�.");
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

// ���ڿ��� ã�� : ���� �ε��� ��ȯ
// ���ڿ��� ��ã�� : -1�� ��ȯ
int String::Find(const char* str) const {
	return Find(0, m_iLen - 1, str);
}

int String::Find(const String& str) const {
	return Find(str.m_pBuffer);
}

// ���ڿ��� startIdx(�����ε��� - ����)���� endIdx(�����ε��� - ����) �����Ͽ� str���ڿ��� ���� ����� ��ġ �ε����� ��ȯ���ݴϴ�.
// Find �Լ��� ����� ������ ���������� Ž�� ������ �ݴ��Դϴ�.
// O(n)
int String::FindReverse(int startIdx, int endIdx, const char* str) const {
	char* pSrc = m_pBuffer + endIdx;

	int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = endIdx;

	if (iStrLen == 0) {
		return -1;
	}

	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("�ε��� ������ ������ϴ�.");
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


// @���� : https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
void String::Format(const char* format, ...) {
	va_list args;
	va_start(args, format);

	int iExpectedLen = vsnprintf(nullptr, 0, format, args); // ���� ��ȯ�� �ʿ��� ���ڿ� ���̸� ȹ��

	if (iExpectedLen <= 0) {
		throw RuntimeException("���ڿ� ���� ������ ������ �߻��Ͽ����ϴ�.");
	}

	if (m_iCapacity < iExpectedLen + 1) {
		Resize(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	vsnprintf(m_pBuffer, m_iCapacity, format, args);
	m_pBuffer[iExpectedLen] = NULL;
	m_iLen = iExpectedLen;
	
	va_end(args);
}

// ���ڿ� ����(m_pBuffer)���� from ���ڿ��� �˻��Ͽ� to ���ڿ��� ��ȯ�մϴ�.
// O(n)
void String::ReplaceAll(const char* from, const char* to) {
	int iFromLen = StringUtil::Length(from);
	
	if (iFromLen == 0) {
		*this = to;
		return;
	}

	std::vector<int> iFindOffsets = FindAll(from);
	int iToLen = StringUtil::Length(to);

	if (iFindOffsets.size() == 0) {
		return;
	}

	// ���̰� ���� ��� �׳� �ٲٸ� �ȴ�.
	if (iFromLen == iToLen) {
		ReplaceAllWithEqualLen(to, iFromLen, iFindOffsets);
		return;
	}

	ReplaceAllWithDifferentLen(from, to, iFromLen, iToLen);
}

void String::ReplaceAllWithEqualLen(const char* to, const int len, std::vector<int>& offsets) {
	for (int i = 0; i < offsets.size(); i++) {
		Memory::CopyUnsafe(&m_pBuffer[offsets[i]], to, len);
	}
}

void String::ReplaceAllWithDifferentLen(const char* from, const char* to, const int fromLen, const int toLen) {
	std::vector<String> vecTokens = Split(from, true);

	int iExceededSize = toLen * vecTokens.size() - fromLen * vecTokens.size();		// �߰��� Ȯ��Ǿ���� ���� ũ��
	int iExpectedCapaity = iExceededSize + m_iLen + 1;

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

void String::SetAt(const int idx, const char ch) {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("�ε����� ������ ������ϴ�.");
	}

	m_pBuffer[idx] = ch;
}

const char String::GetAt(const int idx) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("�ε����� ������ ������ϴ�.");
	}

	return m_pBuffer[idx];
}

String String::GetRange(const int startIdx, const int endIdx) const {
	return String(GetRangeUnsafe(startIdx, endIdx));
}

// ���� ���ڿ��� �����ε���(����)���� �����ε���(����)������ �κ� ���ڿ��� ��ȯ�մϴ�.
char* String::GetRangeUnsafe(const int startIdx, const int endIdx) const {
	if (!IsValidIndexRange(startIdx, endIdx)) {
		throw OutOfRangeException("�ùٸ��� ���� �ε��� �����Դϴ�.");
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

// delimiter ���ڿ� �������� �и��մϴ�.
// includeEmpty�� true�� ��� �и��� ��ū ���ڿ��� ����ִ��� ���� ��ŵ�ϴ�.
// O(n)
std::vector<String> String::Split(const char* delimiter, const bool includeEmpty) const {
	std::vector<String> vecTokens;
	int iOffset = Find(delimiter);

	if (iOffset == -1) {
		vecTokens.emplace_back(m_pBuffer);
		return vecTokens;
	}

	int iDelimiterLen = StringUtil::Length(delimiter);
	if (iOffset - 1 < 0) {
		if (includeEmpty) {
			vecTokens.emplace_back(EMPTY);
		}
	} else {
		vecTokens.emplace_back(GetRangeUnsafe(0, iOffset - 1));
	}

	iOffset += iDelimiterLen;

	while (iOffset < m_iLen) {
		int iNextOffset = Find(iOffset, m_iLen - 1, delimiter);

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

char& String::operator[](const int idx) {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("�ε����� ������ ������ϴ�.");
	}

	return m_pBuffer[idx];
}

String String::operator+(const String& other) {
	String temp = *this;
	temp.Append(other);
	return temp;
}

String String::operator+(const char ch) {
	String temp = *this;
	temp.Append(ch);
	return temp;
}

String String::operator+(const char* str) {
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

// �þ�Դ� C++ �̵� ������ & �̵� ���� ������
// @���� : https://modoocode.com/227
String& String::operator=(String&& other) noexcept {
	// �̵� ������ �н� �� ����
	m_iCapacity = other.m_iCapacity;
	m_iLen = other.m_iLen;
	m_pBuffer = other.m_pBuffer;

	other.m_pBuffer = nullptr;
	return *this;
}

String& String::operator=(const char* other) {
	int iToLen = StringUtil::Length(other);
	int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > m_iCapacity) {
		Initialize(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
	}

	StringUtil::Copy(m_pBuffer, m_iCapacity, other);
	m_iLen = iToLen;

	return *this;
}

bool String::operator==(const String& other) {
	return Compare(other) == 0;
}

bool String::operator==(const char* other) {
	return Compare(other) == 0;
}

bool String::operator!=(const String& other) {
	return Compare(other) != 0;
}

bool String::operator!=(const char* other) {
	return Compare(other) != 0;
}

bool String::operator<(const String& other) {
	return Compare(other) < 0;
}

bool String::operator<(const char* other) {
	return Compare(other) < 0;
}

bool String::operator>(const String& other) {
	return Compare(other) > 0;
}

bool String::operator>(const char* other) {
	return Compare(other) > 0;
}

bool String::operator<=(const String& other) {
	return Compare(other) <= 0;
}

bool String::operator<=(const char* other) {
	return Compare(other) <= 0;
}

bool String::operator>=(const String& other) {
	return Compare(other) >= 0;
}

bool String::operator>=(const char* other) {
	return Compare(other) >= 0;
}

std::ostream& operator<<(std::ostream& os, const String& src) {
	os << src.m_pBuffer;
	return os;
}


} // namespace JCore

