/*
	작성자 : 윤정도
	컴파일 타임 문자열
	런타임에는 String을 쓰면댐 굳이 이거 쓸 필요없음
*/

#pragma once

#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Type.h>
#include <JCore/Exception.h>
#include <JCore/Assert.h>

#pragma warning(push)
  #pragma warning (disable : 4200) // nonstandard extension used : zero - sized array in struct / union -> Source[0] 같은거

NS_JC_BEGIN

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

	constexpr bool operator==(const String& str) const {
		return StringUtil::CTCompare(Source, str.Source()) == 0;
	}

	constexpr bool operator!=(const String& str) const {
		return StringUtil::CTCompare(Source, str.Source()) != 0;
	}

	template <Int32U ParamSize>
	constexpr bool operator==(const StaticString<ParamSize>& str) const {
		return Compare(str) == 0;
	}

	template <Int32U ParamSize>
	constexpr bool operator!=(const StaticString<ParamSize>& str) const {
		return Compare(str) != 0;
	}

	constexpr char& operator[](const int idx) const {
		return *const_cast<char*>(Source + idx);
	}

	constexpr int Length() const {
		return StringUtil::CTLength2(Source);
	}

	constexpr int LengthWithNull() const {
		return StringUtil::CTLength2(Source) + 1;
	}

	static constexpr int Capacity() {
		return Size;
	}

	constexpr int Compare(const char* str) const {
		return StringUtil::CTCompare(Source, str);
	}

	template <Int32U ParamSize>
	constexpr int Compare(const StaticString<ParamSize>& str) const {
		return StringUtil::CTCompare(Source, str.Source);
	}

	template <Int32U ParamSize>
	constexpr void CopyFrom(const char(&str)[ParamSize]) const {
		CopyFrom(0, StringUtil::CTLength2(str) - 1, str);
	}

	template <Int32U ParamSize>
	constexpr void CopyFrom(int startIdx, int endIdx, const char(&str)[ParamSize]) const {
		const int iThisLen = Size - 1;
		int iCopyCount = 0;

		char* pDst = (char*)Source;
		char* pSrc = (char*)str;
		pSrc += startIdx;

		for (int i = startIdx; i <= endIdx && iCopyCount < iThisLen; i++) {
			*pDst = *pSrc;

			++pDst;
			++pSrc;
			iCopyCount++;
		}

		*pDst = '\0';
	}


	template <Int32U ParamSize>
	constexpr bool StartWith(const char(&str)[ParamSize]) const {
		const int iStrLen = ParamSize - 1;
		const int iThisLen = Length();

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
		const int iStrLen = ParamSize - 1;
		const int iThisLen = Length();

		if (iStrLen > iThisLen) {
			return false;
		}

		char* pStr = (char*)str + iStrLen  - 1;
		char* pThis = (char*)Source + iThisLen - 1;

		for (int i = 0; i < iStrLen; i++) {
			if (*pStr != *pThis) {
				return false;
			}

			--pStr;
			--pThis;
				
		}
		
		return true;
	}

	
	template <Int32U ParamSize>
	constexpr int Find(const char(&str)[ParamSize]) const {
		return Find(0, Size - 2, str);
	}

	template <Int32U ParamSize>
	constexpr int Find(const int startIdx, const int endIdx, const char(&str)[ParamSize]) const {
		if (str[0] == '\0') {
			return startIdx;
		}

		char* pSrc = (char*)Source + startIdx;

		const int iStrLen = StringUtil::CTLength(str);
		int iContinuousCount = 0;
		int iOffset = startIdx;

		if (iStrLen == 0) {
			return -1;
		}

		while (iOffset <= endIdx) {
			char* pDst = (char*)str;

			while (*pDst != NULL && *pSrc == *pDst) {
				iContinuousCount++;
				++pSrc;
				++pDst;
			}

			if (iContinuousCount == iStrLen && iOffset + iContinuousCount - 1 <= endIdx) {
				return iOffset;
			}

			if (iContinuousCount == 0) {
				++pSrc;
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
	constexpr StaticString<Size> Remove(const char(&str)[ParamSize]) const {
		const int iStartIdx = Find(str);

		StaticString<Size> ret{};
		ret.Source[Size - 1] = '\0';

		if (iStartIdx == -1) {
			return ret;
		}

		const int iEndIdx = iStartIdx + ParamSize - 2;

		char* pDst = (char*)ret.Source;
		char* pThis = (char*)Source;
		
		for (int i = 0; i < Size - 1; i++, ++pThis) {
			if (i >= iStartIdx && i <= iEndIdx) {
				continue;
			}

			*pDst = *pThis;
			++pDst;
		}

		return ret;
	}

	/*==================================================================
	 non-constexpr methods
	===================================================================*/

	StaticString<Size>& operator=(const char* str) {
		int iCopySize = StringUtil::Copy(Source, Size, str);
		DebugAssertMsg(iCopySize != -1, "복사에 실패했습니다.");
		return *this;
	}

	StaticString<Size>& operator=(const String& str) {
		int iCopySize = StringUtil::Copy(Source, Size, str.Source());
		DebugAssertMsg(iCopySize != -1, "복사에 실패했습니다.");
		return *this;
	}


	int SetStringUnsafe(const char* str) {
		return StringUtil::CopyUnsafe(Source, str);
	}

	int SetStringUnsafe(const String& str) {
		return StringUtil::CopyUnsafe(Source, str.Source());
	}

	int SetStringUnsafe(const std::string& str) {
		return StringUtil::CopyUnsafe(Source, str.c_str());
	}

	template <Int32U ParamSize>
	int SetStringUnsafe(const StaticString<ParamSize>& str) {
		return StringUtil::CopyUnsafe(Source, str.Source);
	}

	int SetString(const char* str) {
		return StringUtil::Copy(Source, Size, str);
	}

	int SetString(const String& str) {
		return StringUtil::Copy(Source, Size, str.Source());
	}

	template <Int32U ParamSize>
	int SetString(const StaticString<ParamSize>& str) {
		return StringUtil::Copy(Source, Size, str.Source);
	}

	int SetString(const char* str, int count) {
		return StringUtil::Copy(Source, count, str);
	}

	int SetString(const String& str, int count) {
		return StringUtil::Copy(Source, count, str.Source());
	}

	template <Int32U ParamSize>
	int SetString(const StaticString<ParamSize>& str, int count) {
		return StringUtil::Copy(Source, count, str.Source);
	}


	String ToString() {
		return Source;
	}

	void CopyFrom(const char* str) {
		const int iLen = StringUtil::Length(str);

		if (iLen <= 0) {
			Source[0] = '\0';
			return;
		}

		CopyFrom(0, iLen - 1, str);
	}

	void CopyFrom(const String& str) {
		if (str.Length() <= 0) {
			Source[0] = '\0';
			return;
		}

		CopyFrom(0, str.Length() - 1, str.Source());
	}

	void CopyFrom(int startIdx, int endIdx, const char* str) {
		if (startIdx > endIdx || startIdx < 0) {
			throw InvalidArgumentException("인덱스 범위가 올바르지 않습니다.");
		}

		const int iThisLen = Size - 1;
		int iCopyIdx = 0;

		for (int i = startIdx; i <= endIdx && iCopyIdx < iThisLen; i++) {
			Source[iCopyIdx++] = str[i];
		}
		Source[iCopyIdx] = '\0';
	}


	template <Int32U ParamSize>
	friend std::ostream& operator<<(std::ostream& os, const StaticString<ParamSize>& str);
};


template<Int32U ParamSize>
std::ostream& operator<<(std::ostream& os, const StaticString<ParamSize>& str) {
	return os << str.Source;
}

NS_JC_END

#pragma warning(pop)