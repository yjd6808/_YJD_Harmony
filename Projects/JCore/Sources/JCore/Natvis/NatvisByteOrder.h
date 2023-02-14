#pragma once

#include <JCore/Namespace.h>
#include <JCore/Type.h>

// 23년 02월 12일 일요일 오후 4시
// 바이트 오더 변환용도

NS_JC_BEGIN
NS_DETAIL_BEGIN

// 일단 포트 변환땜에 하나만 만듬
union NatvisByteOrderInt16U {
	struct
	{
		Int8U Seg1;	// 0xbb
		Int8U Seg2;	// 0xaa
	} Segs;

	Int16U Val; // 0xaabb
};

NS_DETAIL_END
NS_JC_END
