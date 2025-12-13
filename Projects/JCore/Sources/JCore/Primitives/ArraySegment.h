/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:28:24 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Assert.h>
#include <JCore/Define.h>

NS_JC_BEGIN

template <typename T>
class CArraySegment
{
public:
	CArraySegment(T* _pRawArray, int _length)
		: pRawArray_(_pRawArray)
		, length_(_length)
	{
	}

	virtual ~CArraySegment() = default;

	T& operator[](const int _index)
	{
		DebugAssertMsg(_index >= 0 && _index < length_, "세그먼트의 인덱스 범위가 이상합니다.");
		return pRawArray_[_index];
	}

private:
	T* pRawArray_;
	int length_;
};

using ByteArraySegment = CArraySegment<Byte>;

NS_JC_END
