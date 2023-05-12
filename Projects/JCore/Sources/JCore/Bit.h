/*
 * 작성자 : 윤정도
 * 생성일: 2/20/2023 04:23:44 AM
 */

#pragma once

#include <JCore/Namespace.h>
#include <JCore/Limit.h>
#include <JCore/Type.h>

NS_JC_BEGIN

// 오른쪽부터 1을 몇번 채울지
// FillBitRight<5>() -> ... (생략) 0001 1111
// FillBitRight<3>() -> ... (생략) 0000 0111
template <Int32U FillCount>
constexpr int FillBitRight32() {
	return (1 << FillCount) - 1;
}

// 왼쪽부터 1을 몇번 채울지
// FillBitLeft<5>() -> 1111 1000 ... 생략
// FillBitLeft<3>() -> 1110 0000 ... 생략
template <Int32U FillCount>
constexpr int FillBitLeft32() {
	return MaxInt32U_v & ~FillBitRight32<BitCount<int>() - FillCount>();
}


template <Int32U FillCount>
constexpr int FillBitRight32(int value) {
	return value | FillBitRight32<FillCount>();
}

template <Int32U FillCount>
constexpr int FillBitLeft32(int value) {
	return value | FillBitLeft32<FillCount>();
}


constexpr int Bit32(int pos) {
	return 1 << pos;
}

constexpr int BitSet32(int source, int value) {
	return source | value;
}

constexpr int BitRemove32(int source, int value) {
	return source &= ~value;
}

constexpr int BitIsSet32(int source, int target) {
	return (source & target) == target;
}

NS_JC_END
