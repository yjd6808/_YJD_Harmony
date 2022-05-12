/*
	�ۼ��� : ������
	operator �Լ� �н��� ���� �����غ� ���ڿ� Ŭ�����Դϴ�.
*/

#include <JCore/Core.h>
#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/Memory.h>
#include <JCore/Exception.h>
#include <JCore/Ascii.h>
#include <JCore/Tuple.h>

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
		throw NullPointerException("���ڿ��� nullptr �Դϴ�.");
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
	// �̵� ���� ������ ȣ��
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
		throw NullPointerException("�߰��ϰ��� �ϴ� ���ڿ��� nullptr �Դϴ�.");
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

	// ���ڿ� ���̸�ŭ �ڷ� �о���
	// �������ڿ��� ������ ����ְų�, �ǵڿ� �����ϴ� ���
	// m_iLen - idx�� 0�� �ǹ����� �ƿ� �����C �����Ͱ� ���� ��찡 ���� �� �ִ�.
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

// len���̰� ���� capacity�� �ʰ��� ��� Ȯ���� ���ش�.
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

// 0  : ����
// 1  : ���� ���ڿ�(this)�� �켱������ �� ŭ
// -1 : �� ���ڿ�(str)�� �켱������ �� ŭ
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

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

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

	const int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = startIdx;

	if (iStrLen == 0) {
		return -1;
	}

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

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

int String::Find(int startIdx, const char* str) const {
	return Find(startIdx, m_iLen - 1, str);
}

// ���ڿ��� ã�� : ���� �ε��� ��ȯ
// ���ڿ��� ��ã�� : -1�� ��ȯ
int String::Find(const char* str) const {
	return Find(0, m_iLen - 1, str);
}

int String::Find(const String& str) const {
	return Find(str.m_pBuffer);
}

int String::Find(int startIdx, const String& str) const {
	return Find(startIdx, m_iLen - 1, str.Source());
}

// ���ڿ��� startIdx(�����ε��� - ����)���� endIdx(�����ε��� - ����) �����Ͽ� str���ڿ��� ���� ����� ��ġ �ε����� ��ȯ���ݴϴ�.
// Find �Լ��� ����� ������ ���������� Ž�� ������ �ݴ��Դϴ�.
// O(n)
int String::FindReverse(int startIdx, int endIdx, const char* str) const {
	char* pSrc = m_pBuffer + endIdx;

	const int iStrLen = StringUtil::Length(str);
	int iContinuousCount = 0;
	int iOffset = endIdx;

	if (iStrLen == 0) {
		return -1;
	}

	ThrowIfInvalidRangeIndex(startIdx, endIdx);

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

void String::Clear() {
	ThrowIfNotInitialized();
	m_iLen = 0;
	m_pBuffer[m_iLen] = NULL;
}

void String::Clear(int offset, int len) {
	ThrowIfInvalidIndex(offset);

	const int iRemoveLen = offset + len > m_iLen ? m_iLen - offset : len;	// ������� abcdefg���� offset 6�� len 100�� �̷��� ������ len�� 1�� �ǵ��� �������
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
 * \brief  startIdx(����) ~ endIdx (endIdx)������ str���ڿ� ������ ��ȯ�ع���!
 * O(n)
 */
int String::Count(const int startIdx, const int endIdx, const char* str) const {
	ThrowIfNotInitialized();
	ThrowIfInvalidRangeIndex(startIdx, endIdx);

	int iOffset = 0;
	int iCount = 0;

	while (iOffset <= endIdx && (iOffset = Find(iOffset, endIdx, str)) != -1) {
		iCount++;
		iOffset++;
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
	const int iLen = offset + len > m_iLen ? m_iLen - offset : len;	// ������� abcdefg���� offset 6�� len 100�� �̷��� ������ len�� 1�� �ǵ��� �������

	
	if (iLen < to.Length()) {
		// ġȯ�ϰ����ϴ� ���ڿ��� ���̰� �� �� ��� �� ��ŭ�� �ڷ� �о��� ���� ���̿� �ϳ��ϳ� �������ش�.
		ResizeIfNeeded(m_iLen + to.Length() - iLen);	// Ȯ��������� �� ����

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
		// ġȯ�ϰ����ϴ� ���ڿ��� ���̰� ���ų� ª�� ��� 
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

	return offset + to.Length();
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

	const int iExpectedLen = vsnprintf(nullptr, 0, format, args); // ���� ��ȯ�� �ʿ��� ���ڿ� ���̸� ȹ��

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

	// ���̰� ���� ��� �׳� �ٲٸ� �ȴ�.
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

	const int iExceededSize = toLen * vecTokens.size() - fromLen * vecTokens.size();		// �߰��� Ȯ��Ǿ���� ���� ũ��
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

// ���� ���ڿ��� �����ε���(����)���� �����ε���(����)������ �κ� ���ڿ��� ��ȯ�մϴ�.
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

	const int iDelimiterLen = StringUtil::Length(delimiter);
	if (iOffset - 1 < 0) {
		if (includeEmpty) {
			vecTokens.emplace_back(EMPTY);
		}
	} else {
		vecTokens.emplace_back(GetRange(0, iOffset - 1));
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
			vecTokens.emplace_back(GetRange(iOffset, iNextOffset - 1));
		}
		iOffset = iNextOffset + 1;
	}

	if (iOffset < m_iLen) {
		vecTokens.emplace_back(GetRange(iOffset, m_iLen - 1));
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

// �þ�Դ� C++ �̵� ������ & �̵� ���� ������
// @���� : https://modoocode.com/227
String& String::operator=(String&& other) noexcept {
	// ���� ���ڿ� �����������
	DeleteArraySafe(m_pBuffer);

	// �̵� ������ �н� �� ����
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
		throw OutOfRangeException("�ε��� ������ ������ϴ�.");
	}
}

void String::ThrowIfNotInitialized() const {
	if (m_pBuffer == nullptr) {
		throw NullPointerException("String�� ���� �ʱ�ȭ���ּ���.");
	}
}

void String::ThrowIfInvalidIndex(const int idx) const {
	if (!IsValidIndex(idx)) {
		throw OutOfRangeException("�ε����� ������ ������ϴ�.");
	}
}


} // namespace JCore


