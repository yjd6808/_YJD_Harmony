/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#include <JCore/Core.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Exception.h>
#include <JCore/Container/Vector.h>

NS_JC_BEGIN

Vector<String, DefaultAllocator> StringUtil::Split(String& src, const char* delimiter) {
	return src.Split(delimiter);
}

String StringUtil::Format(const char* format, ...) {
	va_list args;
	va_start(args, format);
	String szResult = Format(format, args);
	va_end(args);
	return szResult;
}

String StringUtil::Format(const char* format, va_list args) {
	const int iExpectedLen = vsnprintf(nullptr, 0, format, args); // 포맷 변환시 필요한 문자열 길이를 획득

	if (iExpectedLen <= 0) {
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	String szResult(iExpectedLen + 1 + String::DefaultBufferSize);
	vsnprintf(szResult.Source(), szResult.Capacity(), format, args);
	szResult.SetAt(iExpectedLen, NULL);
	szResult.m_iLen = iExpectedLen;
	return szResult;
}

void StringUtil::FormatBuffer(char* buff, const int buffCapacity, const char* format, ...) {
	va_list args;
	va_start(args, format);
	FormatBuffer(buff, buffCapacity, format, args);
	va_end(args);
}

void StringUtil::FormatBuffer(char* buff, const int buffCapacity, const char* format, va_list args) {
	const int iExpectedLen = vsnprintf(nullptr, 0, format, args); // 포맷 변환시 필요한 문자열 길이를 획득
	DebugAssertMsg(iExpectedLen > 0, "문자열 포맷 수행중 오류가 발생하였습니다.");
	DebugAssertMsg(iExpectedLen < buffCapacity, "문자열 포맷 수행중 오류가 발생하였습니다. (문자열 길이가 버퍼의 용량을 초과합니다.)");
	vsnprintf(buff, buffCapacity, format, args);
}


bool StringUtil::IsNullOrEmpty(const char* str) {
	if (str == nullptr) return true;
	if (str[0] == '\0') return true;
	return false;
}

int StringUtil::Length(const char* str) {
	if (str == nullptr) {
		return -1;
	}

	int iSize = 0;
	while (*str != NULL) {
		iSize++;
		str++;
	}
	return iSize;
}


int StringUtil::Copy(char* buffer, const int bufferSize, const char* copy) {
	if (buffer == nullptr || copy == nullptr) {
		return -1;
	}

	int iSize = 0;
	char* pBuffer = buffer;

	while (*copy != NULL && iSize < bufferSize) {
		*buffer = *copy;
		buffer++;
		copy++;
		iSize++;
	}

	pBuffer[iSize] = NULL;
	return iSize;
}

int StringUtil::CopyUnsafe(char* buffer, const char* copy) {
	if (buffer == nullptr || copy == nullptr) {
		return -1;
	}

	int iSize = 0;
	char* pBuffer = buffer;

	while (*copy != NULL) {
		*buffer = *copy;
		buffer++;
		copy++;
		iSize++;
	}

	pBuffer[iSize] = NULL;
	return iSize;
}

bool StringUtil::IsEqual(const char* src, const int srcLen, const char* dst, const int dstLen) {
	for (int i = 0, j = 0; i < srcLen && j < dstLen; i++, j++) {
		if (*(src + i) != *(dst + j)) {
			return false;
		}
	}
	return true;
}

void StringUtil::Swap(String& src, String& dst) {
	String temp = std::move(src);
	src = std::move(dst);
	dst = std::move(temp);
}

int StringUtil::Find(const char* source, int sourceLen, int startIdx, int endIdx, const char* str) {
	return Find(source, sourceLen, startIdx, endIdx, str, Length(str));
}

const char* StringUtil::SkipLeadingChar(const char* str, char skipChar) {
	const int iPos = FindCharUncontained(str, skipChar);
	return str + iPos;
}

const char* StringUtil::SkipLeadingNumberZero(const char* str) {
	
	const int iStrLen = Length(str);

	if (iStrLen == 0) {
		return str;
	}

	// 전부 0인경우가 있을 수 있으므로 전체 스킵된 경우 0으로 처리되도록 한다.
	const char* szSkippedLeadingZero = SkipLeadingChar(str, '0');
	if (szSkippedLeadingZero == str + iStrLen) {
		return "0";
	}

	return szSkippedLeadingZero;
}

int StringUtil::FindChar(const char* source, char ch) {
	int i = 0;

	while (true) {
		const char sch = *(source + i);

		if (sch == NULL) {
			return -1;
		}

		if (sch == ch) {
			return i;
		}

		++i;
	}
}

int StringUtil::FindCharReverse(const char* source, char ch) {
	return FindCharReverse(source, Length(source), ch);
}

int StringUtil::FindCharReverse(const char* source, int len, char ch) {
	while ((--len) >= 0) {
		if (source[len] == ch)
			return len;
	}
	return -1;
}

int StringUtil::FindCharUncontained(const char* source, char ch) {
	int i = 0;

	while (true) {
		const char sch = *(source + i);

		if (sch == NULL) {
			return i;
		}

		if (sch != ch) {
			return i;
		}

		++i;
	}
}

// 문자열의 startIdx(시작인덱스 - 포함)부터 endIdx(종료인덱스 - 포함) 포함하여 str문자열이 있을 경우의 위치 인덱스를 반환해줍니다.
// O(n)
int StringUtil::Find(const char* source, int sourceLen, int startIdx, int endIdx, const char* str, int strLen) {
	const int iFindStrLen = strLen;
	const int iSearchLen = endIdx - startIdx + 1;

	if (strLen == 0) {
		return 0;
	}

	if (startIdx > endIdx || startIdx < 0 || endIdx >= sourceLen) {
		return -1;
	}

	if (iFindStrLen > iSearchLen) {
		return -1;
	}

	for (int i = startIdx; i <= endIdx; i++) {
		int iContinuous = 0;

		while (iContinuous < iFindStrLen && source[i + iContinuous] == str[iContinuous]) {
			iContinuous++;
		}

		if (iContinuous == iFindStrLen) {
			return i;
		}
	}

	return -1;
}

int StringUtil::Find(const char* source, int sourceLen, int startIdx, const char* str) {
	return Find(source, sourceLen, startIdx, sourceLen - 1, str);
}

int StringUtil::FindAll(JCORE_OUT int* positionArray, const char* source, const char* str) {
	const int iSourceLength = Length(source);
	return FindAll(positionArray, source, iSourceLength, 0, iSourceLength - 1, str);
}

int StringUtil::FindAll(int* positionArray, const char* source, int sourceLen, const char* str) {
	return FindAll(positionArray, source, sourceLen, 0, sourceLen - 1, str);
}

int StringUtil::FindAll(int* positionArray, const char* source, int sourceLen, int startIdx, int endIdx, const char* str) {
	if (endIdx < startIdx) {
		return 0;
	}

	int iCount = 0;
	int iFindPos = -1;
	int iStrLength = Length(str);

	while ((iFindPos = Find(source, sourceLen, startIdx, endIdx, str, iStrLength)) != -1) {
		positionArray[iCount++] = iFindPos;
		startIdx = iFindPos + iStrLength;
	}

	return iCount;
}

String StringUtil::GetRange(const char* source, int sourceLen, int startIdx, int endIdx) {
	String subString{0};

	auto [pBuffer, iLen, iCapacity] = GetRangeUnsafe(source, sourceLen, startIdx, endIdx);

	subString.m_pBuffer = pBuffer;
	subString.m_iLen = iLen;
	subString.m_iCapacity = iCapacity;

	return subString;
}

Tuple<char*, int, int> StringUtil::GetRangeUnsafe(const char* source, int sourceLen, int startIdx, int endIdx) {
	if (startIdx > endIdx || startIdx < 0 || endIdx >= sourceLen) {
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}

	const char* pStr = source + startIdx;
	int iCurIdx = startIdx;
	int iIdx = 0;
	const int iAllocCapacity = endIdx - startIdx + 10;
	char* szRange = dbg_new char[iAllocCapacity];

	while (iCurIdx <= endIdx) {
		szRange[iIdx] = source[iCurIdx];
		iCurIdx++;
		iIdx++;
	}

	szRange[iIdx] = NULL;
	return { szRange, iIdx, iAllocCapacity };
}


void StringUtil::ConcatInnerBack(char* buf, int buflen, int bufCapacity, const char* concatStr, int concatStrLen) {
	DebugAssertMsg(buflen + concatStrLen + 1 <= bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafe(buf + buflen, concatStr, concatStrLen);
	buf[buflen + concatStrLen] = NULL;

}

void StringUtil::ConcatInnerBack(char* buf, int bufCapacity, const char* cancatStr) {
	int iBufLen = Length(buf);
	int iConcatLen = Length(cancatStr);
	ConcatInnerBack(buf, iBufLen, bufCapacity, cancatStr, iConcatLen);
}

void StringUtil::ConcatInnerFront(char* buf, int buflen, int bufCapacity, const char* concatStr, int concatStrLen) {
	DebugAssertMsg(buflen + concatStrLen + 1 <= bufCapacity, "버퍼 용량을 초과할 수 없습니다.");
	Memory::CopyUnsafeReverse(buf + concatStrLen, buf, buflen);
	Memory::CopyUnsafe(buf, concatStr, concatStrLen);
	buf[buflen + concatStrLen] = NULL;
}

void StringUtil::ConcatInnerFront(char* buf, int bufCapacity, const char* concatStr) {
	int iBufLen = Length(buf);
	int iConcatLen = Length(concatStr);
	ConcatInnerFront(buf, iBufLen, bufCapacity, concatStr, iConcatLen);
}

// https://github.com/otland/forgottenserver/blob/545516299b607ef25471f84d1805a22311ab72de/src/pugicast.h
template <> float StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtof(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr); }
template <> double StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtod(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr); }
template <> Int32L StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtol(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0); }
template <> Int64 StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtoll(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0); }
template <> Int32UL StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtoul(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0); }
template <> Int64U StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return std::strtoull(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0); }
template <> Char StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Char>(std::strtol(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> WideChar StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<WideChar>(std::strtoul(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> Byte StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Byte>(std::strtoul(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> Int16 StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Int16>(std::strtol(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> Int16U StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Int16U>(std::strtoul(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> Int32 StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Int32>(std::strtol(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }
template <> Int32U StringUtil::ToNumber(const char* str, bool ignoreLeadingZero) { return static_cast<Int32U>(std::strtoul(ignoreLeadingZero ? SkipLeadingNumberZero(str) : str, nullptr, 0)); }

template <> String StringUtil::ToString(bool value) { return value ? "true" : "false"; }
template <> String StringUtil::ToString(float value) { return std::to_string(value); }
template <> String StringUtil::ToString(double value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int32L value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int64 value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int32UL value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int64U value) { return std::to_string(value); }
template <> String StringUtil::ToString(Char value) { return std::to_string(value); }
template <> String StringUtil::ToString(Byte value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int16 value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int16U value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int32 value) { return std::to_string(value); }
template <> String StringUtil::ToString(Int32U value) { return std::to_string(value); }


/*
constexpr Int64 StringUtil::ConvertInt64(const char* src, const int len) {

	return 0;
}

template <typename W>
inline constexpr W StringUtil::Getter() {
	return W();
}

template<Int32U Size>
inline constexpr auto StringUtil::TrimFront(const char(&str)[Size], const char delimiter) {
	constexpr int iLen = Size - 1;
	int iTrimEndIdx = -1;
	for (int i = 0; i < iLen; i++) {
		if (str[i] != delimiter) {
			iTrimEndIdx = i;
		}
	}

	StaticString<iLen> ret{};
	char* ptr = (char*)ret.Source;

	// 트림이 어디서 끝나는지 알 수 없으므로 전부 널로 넣어주자
	for (int i = 0; i < iLen; i++) {
		*ptr = NULL;
		ptr++;
	}

	ptr = (char*)ret.Source;

	for (int i = iTrimEndIdx; i < iLen; i++) {
		*ptr = str[i];
		ptr++;
	}

	return ret;
}

*/

NS_JC_END

