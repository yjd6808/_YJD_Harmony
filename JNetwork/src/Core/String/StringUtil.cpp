/*
	작성자 : 윤정도
	문자열 조작을 도와주는 클래스입니다.
*/

#include <Core/Core.h>
#include <Core/String/String.h>
#include <Core/String/StringUtil.h>

namespace JCore {

std::vector<String> StringUtil::Split(String& src, const char* delimiter)
{
	return src.Split(delimiter);
}

String StringUtil::Format(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int iExpectedLen = vsnprintf(nullptr, 0, format, args); // 포맷 변환시 필요한 문자열 길이를 획득

	if (iExpectedLen <= 0) {
		throw std::runtime_error("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	String szResult(iExpectedLen + 1 + String::DEFAULT_BUFFER_SIZE);
	vsnprintf(szResult.Source(), szResult.Capacity(), format, args);
	szResult.SetAt(iExpectedLen, NULL);
	va_end(args);
	return szResult;
}

const int StringUtil::Length(const char* str)
{
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


const int StringUtil::Copy(char* buffer, const int bufferSize, const char* copy)
{
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

const bool StringUtil::IsEqual(const char* src, const int srcLen, const char* dst, const int dstLen)
{
	for (int i = 0, j = 0; i < srcLen && j < dstLen; i++, j++) {
		if (*(src + i) != *(dst + j)) {
			return false;
		}
	}
	return true;
}

void StringUtil::Swap(String& src, String& dst)
{
	String temp = std::move(src);
	src = std::move(dst);
	dst = std::move(temp);
}

} // namespace JCore

