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
class ArraySegment
{
public:
	ArraySegment(T* arr, int len)
		: m_pRawArray(arr)
		, m_iLen(len) {}
	virtual ~ArraySegment() = default;

	T& operator[](const int idx) {
		DebugAssertMsg(idx >= 0 && idx < m_iLen, "세그먼트의 인덱스 범위가 이상합니다.");
		return m_pRawArray[idx];
	}
private:
	T* m_pRawArray;
	int m_iLen;
};

using ByteArraySegment = ArraySegment<Byte>;

NS_JC_END