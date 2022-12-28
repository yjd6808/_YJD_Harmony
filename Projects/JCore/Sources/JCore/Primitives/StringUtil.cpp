/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#include <JCore/Core.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Exception.h>
#include <JCore/Container/Vector.h>

namespace JCore {

Vector<String, DefaultAllocator> StringUtil::Split(String& src, const char* delimiter) {
	return src.Split(delimiter);
}

String StringUtil::Format(const char* format, ...) {
	va_list args;
	va_start(args, format);

	const int iExpectedLen = vsnprintf(nullptr, 0, format, args); // 포맷 변환시 필요한 문자열 길이를 획득

	if (iExpectedLen <= 0) {
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	String szResult(iExpectedLen + 1 + String::DEFAULT_BUFFER_SIZE);
	vsnprintf(szResult.Source(), szResult.Capacity(), format, args);
	szResult.SetAt(iExpectedLen, NULL);
	szResult.m_iLen = iExpectedLen;
	va_end(args);
	return szResult;
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

} // namespace JCore

