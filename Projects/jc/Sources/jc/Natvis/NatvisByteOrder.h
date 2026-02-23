#pragma once

#include <jc/Namespace.h>
#include <jc/Type.h>

// 23년 02월 12일 일요일 오후 4시
// 바이트 오더 변환용도

NS_JC_BEGIN
NS_DETAIL_BEGIN

// 일단 포트 변환땜에 하나만 만듬
union NatvisByteOrderInt16U
{
	struct Field
	{
		_u8 seg1; // 0xbb
		_u8 seg2; // 0xaa
	} segs;

	_u16 val; // 0xaabb
};

NS_END
NS_END
