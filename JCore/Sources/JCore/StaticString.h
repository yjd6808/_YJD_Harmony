/*
	작성자 : 윤정도
	컴파일 타임 문자열
	런타임에는 String을 쓰면댐 굳이 이거 쓸 필요없음
*/

#pragma once

#include <JCore/String.h>
#include <JCore/Type.h>
#include <JCore/StringUtil.h>

namespace JCore {

template <Int32U Size>
struct StaticString
{
	char Source[Size] = "";

	template <Int32U ParamSize>
	constexpr bool operator==(const char(&str)[ParamSize]) const {
		return StringUtil::CTCompare(Source, str) == 0;
	}

	template <Int32U ParamSize>
	constexpr bool operator!=(const char(&str)[ParamSize]) const {
		return StringUtil::CTCompare(Source, str) != 0;
	}

	constexpr bool operator==(const char* str) const {
		return StringUtil::CTCompare(Source, str) == 0;
	}

	constexpr bool operator!=(const char* str) const {
		return StringUtil::CTCompare(Source, str) != 0;
	}

	template <Int32U ParamSize>
	constexpr bool operator==(const StaticString<ParamSize>& str) const {
		return Compare(str) == 0;
	}

	template <Int32U ParamSize>
	constexpr bool operator!=(const StaticString<ParamSize>& str) const {
		return Compare(str) != 0;
	}

	constexpr int Compare(const char* str) const {
		return StringUtil::CTCompare(Source, str);
	}

	template <Int32U ParamSize>
	constexpr int Compare(const StaticString<ParamSize>& str) const {
		return StringUtil::CTCompare(Source, str.Source);
	}

	template <Int32U ParamSize>
	constexpr bool StartWith(const char(&str)[ParamSize]) const {
		int iStrLen = ParamSize - 1;
		int iThisLen = Size - 1;

		if (iStrLen > iThisLen) {
			return false;
		}
		
		for (int i = 0; i < iStrLen; i++) {
			if (Source[i] != str[i]) {
				return false;
			}
		}

		return true;
	}

	template <Int32U ParamSize>
	constexpr bool EndWith(const char(&str)[ParamSize]) const {
		int iStrLen = ParamSize - 1;
		int iThisLen = Size - 1;

		if (iStrLen > iThisLen) {
			return false;
		}

		char* pStr = (char*)str + iStrLen  - 1;
		char* pThis = (char*)Source + iThisLen - 1;

		for (int i = 0; i < iStrLen; i++) {
			if (*pStr != *pThis) {
				return false;
			}
		}
		
		return true;
	}

	
	template <Int32U ParamSize>
	constexpr int Find(const char(&str)[ParamSize]) const {
		return Find(0, Size - 2, str);
	}

	template <Int32U ParamSize>
	constexpr int Find(const int startIdx, const int endIdx, const char(&str)[ParamSize]) const {
		char* pSrc = (char*)Source + startIdx;

		int iStrLen = StringUtil::CTLength(str);
		int iContinuousCount = 0;
		int iOffset = startIdx;

		if (iStrLen == 0) {
			return -1;
		}

		while (iOffset <= endIdx) {
			char* pDst = (char*)str;

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

	template <Int32U ParamSize>
	constexpr bool Contain(const char(&str)[ParamSize]) const {
		return Find(0, Size - 2, str) != -1;
	}

	// 기존 문자열은 건드릴 수 없기때문에 새로 생성해서 반환함
	template <Int32U ParamSize>
	constexpr auto Remove(const char(&str)[ParamSize]) const {
		int iStartIdx = Find(str);

		StaticString<Size> ret{};
		ret.Source[Size - 1] = '\0';

		if (iStartIdx == -1) {
			return ret;
		}

		int iEndIdx = iStartIdx > iStartIdx + ParamSize - 2;

		char* pSrc = (char*)str;
		char* pDst = (char*)ret.Source;
		char* pThis = (char*)Source;
		
		for (int i = 0; i < Size - 1; i++) {
			*pDst = '\0';
		}

		for (int i = 0; i < Size - 1; i++, pThis++) {
			if (i >= iStartIdx && i <= iEndIdx) {
				continue;
			}

			*pDst = *pThis;
			pDst++;
		}

		return ret;
	}


	String ToString() {
		return Source;
	}

	template <Int32U ParamSize>
	friend std::ostream& operator<<(std::ostream& os, const StaticString<ParamSize>& str);
};


template<Int32U ParamSize>
std::ostream& operator<<(std::ostream& os, const StaticString<ParamSize>& str) {
	return os << str.Source;
}

} // namespace JCore