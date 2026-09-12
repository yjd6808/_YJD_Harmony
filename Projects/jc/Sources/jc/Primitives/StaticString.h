/*
	작성자 : 윤정도
	컴파일 타임 문자열
	런타임에는 String을 쓰면댐 굳이 이거 쓸 필요없음
*/

#pragma once

#include "jc/Primitives/String.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Type.h"
#include "jc/Exception.h"
#include "jc/Assert.h"

#include <ostream>
#include <string>

#pragma warning(push)
  #pragma warning (disable : 4200) // nonstandard extension used : zero - sized array in struct / union -> Source[0] 같은거

NS_JC_BEGIN

template <_u32 Size, typename CharT = _char>
struct StaticString
{
	using StrType = BasicString<CharT, StringImpl_SSO<CharT>>;

	CharT Source[Size] = {};

	template <_u32 ParamSize>
	constexpr bool operator==(const CharT(&_str)[ParamSize]) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _str) == 0;
	}

	template <_u32 ParamSize>
	constexpr bool operator!=(const CharT(&_str)[ParamSize]) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _str) != 0;
	}

	constexpr bool operator==(const CharT* _pStr) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _pStr) == 0;
	}

	constexpr bool operator!=(const CharT* _pStr) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _pStr) != 0;
	}

	constexpr bool operator==(const StrType& _str) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _str.SafeSource()) == 0;
	}

	constexpr bool operator!=(const StrType& _str) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _str.SafeSource()) != 0;
	}

	template <_u32 ParamSize>
	constexpr bool operator==(const StaticString<ParamSize, CharT>& _str) const {
		return Compare(_str) == 0;
	}

	template <_u32 ParamSize>
	constexpr bool operator!=(const StaticString<ParamSize, CharT>& _str) const {
		return Compare(_str) != 0;
	}

	constexpr CharT& operator[](const int _index) {
		return Source[_index];
	}

	constexpr const CharT& operator[](const int _index) const {
		return Source[_index];
	}

	constexpr int Length() const {
		return BasicStringUtil<CharT>::CTLength(Source);
	}

	constexpr int LengthWithNull() const {
		return BasicStringUtil<CharT>::CTLength(Source) + 1;
	}

	static constexpr int Capacity() {
		return Size;
	}

	constexpr int Compare(const CharT* _pStr) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _pStr);
	}

	template <_u32 ParamSize>
	constexpr int Compare(const StaticString<ParamSize, CharT>& _str) const {
		return BasicStringUtil<CharT>::CTCompare(Source, _str.Source);
	}

	template <_u32 ParamSize>
	constexpr void CopyFrom(int _startIndex, int _endIndex, const CharT(&_str)[ParamSize]) {
		const int thisLength = Size - 1;
		int copyCount = 0;

		CharT* pDst = Source;
		const CharT* pSrc = _str;
		pSrc += _startIndex;

		for (int i = _startIndex; i <= _endIndex && copyCount < thisLength; i++) {
			*pDst = *pSrc;

			++pDst;
			++pSrc;
			copyCount++;
		}

		*pDst = CharT(0);
	}


	template <_u32 ParamSize>
	constexpr bool StartWith(const CharT(&str)[ParamSize]) const {
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

	template <_u32 ParamSize>
	constexpr bool EndWith(const CharT(&_str)[ParamSize]) const {
		const int strLength = ParamSize - 1;
		const int thisLength = Length();

		if (strLength > thisLength) {
			return false;
		}

		const CharT* pStr = _str + strLength  - 1;
		const CharT* pThis = Source + thisLength - 1;

		for (int i = 0; i < strLength; i++) {
			if (*pStr != *pThis) {
				return false;
			}

			--pStr;
			--pThis;

		}

		return true;
	}


	template <_u32 ParamSize>
	constexpr int Find(const CharT(&str)[ParamSize]) const {
		return Find(0, Size - 2, str);
	}

	template <_u32 ParamSize>
	constexpr int Find(const int startIdx, const int endIdx, const CharT(&str)[ParamSize]) const {
		if (str[0] == CharT(0)) {
			return startIdx;
		}

		const CharT* pSrc = Source + startIdx;

		const int iStrLen = BasicStringUtil<CharT>::CTLength(str);
		int iContinuousCount = 0;
		int iOffset = startIdx;

		if (iStrLen == 0) {
			return -1;
		}

		while (iOffset <= endIdx) {
			const CharT* pDst = str;

			while (*pDst != CharT(0) && *pSrc == *pDst) {
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

	template <_u32 ParamSize>
	constexpr bool Contain(const CharT(&str)[ParamSize]) const {
		return Find(0, Size - 2, str) != -1;
	}

	// 기존 문자열은 건드릴 수 없기때문에 새로 생성해서 반환함
	template <_u32 ParamSize>
	constexpr StaticString<Size, CharT> Remove(const CharT(&str)[ParamSize]) const {
		const int iStartIdx = Find(str);

		StaticString<Size, CharT> ret{};
		ret.Source[Size - 1] = CharT(0);

		if (iStartIdx == -1) {
			return ret;
		}

		const int iEndIdx = iStartIdx + ParamSize - 2;

		CharT* pDst = ret.Source;
		const CharT* pThis = Source;

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

	StaticString<Size, CharT>& operator=(const CharT* str) {
		int iCopySize = BasicStringUtil<CharT>::Copy(Source, Size, str);
		jc_assert_msg(iCopySize != -1, _T("복사에 실패했습니다."));
		return *this;
	}

	StaticString<Size, CharT>& operator=(const StrType& str) {
		int iCopySize = BasicStringUtil<CharT>::Copy(Source, Size, str.Source());
		jc_assert_msg(iCopySize != -1, _T("복사에 실패했습니다."));
		return *this;
	}


	// 이건 안들어오고 최적화된것마냥 알아서 원소복사를 해주네.
	// StaticString<32> a;
	// StaticString<32> b;
	// s.operator=(b); // 실행해도 안들어옴, 어셈블리 확인하면 하나씩 복사해주는 코드가 알아서들어가는 걸로보인다.
	template <_u32 SrcSize>
	StaticString<Size, CharT>& operator=(const StaticString<SrcSize, CharT>&& str) {
		int iCopySize = BasicStringUtil<CharT>::Copy(Source, Size, str.Source);
		jc_assert_msg(iCopySize != -1, _T("복사에 실패했습니다."));
		return *this;
	}

	// Source의 Capacity는 고려하지 않고 str의 문자들을 모두 복사
	int SetStringUnsafe(const CharT* str) {
		return BasicStringUtil<CharT>::CopyUnsafe(Source, str);
	}

	int SetStringUnsafe(const StrType& str) {
		return BasicStringUtil<CharT>::CopyUnsafe(Source, str.Source());
	}

	int SetStringUnsafe(const std::basic_string<CharT>& str) {
		return BasicStringUtil<CharT>::CopyUnsafe(Source, str.c_str());
	}

	template <_u32 ParamSize>
	int SetStringUnsafe(const StaticString<ParamSize, CharT>& str) {
		return BasicStringUtil<CharT>::CopyUnsafe(Source, str.Source);
	}

	int SetString(const CharT* str) {
		return BasicStringUtil<CharT>::Copy(Source, Size, str);
	}

	int SetString(const StrType& str) {
		return BasicStringUtil<CharT>::Copy(Source, Size, str.Source());
	}

	template <_u32 ParamSize>
	int SetString(const StaticString<ParamSize, CharT>& str) {
		return BasicStringUtil<CharT>::Copy(Source, Size, str.Source);
	}

	int SetString(const CharT* str, int count) {
		const int iCapacity = count < static_cast<int>(Size) ? count : static_cast<int>(Size);
		return BasicStringUtil<CharT>::Copy(Source, iCapacity, str);
	}

	int SetString(const StrType& str, int count) {
		const int iCapacity = count < static_cast<int>(Size) ? count : static_cast<int>(Size);
		return BasicStringUtil<CharT>::Copy(Source, iCapacity, str.Source());
	}

	template <_u32 ParamSize>
	int SetString(const StaticString<ParamSize, CharT>& str, int count) {
		const int iCapacity = count < static_cast<int>(Size) ? count : static_cast<int>(Size);
		return BasicStringUtil<CharT>::Copy(Source, iCapacity, str.Source);
	}


	StrType ToString() {
		return Source;
	}

	void CopyFrom(const CharT* str) {
		const int iLen = BasicStringUtil<CharT>::Length(str);

		if (iLen <= 0) {
			Source[0] = CharT(0);
			return;
		}

		CopyFrom(0, iLen - 1, str);
	}

	void CopyFrom(const StrType& str) {
		if (str.Length() <= 0) {
			Source[0] = CharT(0);
			return;
		}

		CopyFrom(0, str.Length() - 1, str.Source());
	}

	void CopyFrom(int startIdx, int endIdx, const CharT* str) {
		if (startIdx > endIdx || startIdx < 0) {
			throw InvalidArgumentException("인덱스 범위가 올바르지 않습니다.");
		}

		const int iThisLen = Size - 1;
		int iCopyIdx = 0;

		for (int i = startIdx; i <= endIdx && iCopyIdx < iThisLen; i++) {
			Source[iCopyIdx++] = str[i];
		}
		Source[iCopyIdx] = CharT(0);
	}


	template <_u32 ParamSize, typename ParamCharT>
	friend std::basic_ostream<ParamCharT>& operator<<(std::basic_ostream<ParamCharT>& os, const StaticString<ParamSize, ParamCharT>& str);
};


template <_u32 ParamSize, typename CharT>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const StaticString<ParamSize, CharT>& str) {
	return os << str.Source;
}

template <_u32 N> using StaticStringA = StaticString<N, char>;
template <_u32 N> using StaticStringW = StaticString<N, wchar_t>;
template <_u32 N> using StaticStringT = StaticString<N, _char>;

NS_END

#pragma warning(pop)
