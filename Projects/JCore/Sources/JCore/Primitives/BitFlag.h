/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 9:29:26 AM
 * =====================
 * 비트 플래그를 쉽게 수행하기 위함.
 */


#pragma once

#include <JCore/TypeTraits.h>

NS_JC_BEGIN

template <typename TFlag>
struct BitFlag
{
	using TThis = BitFlag<TFlag>;

	static_assert(JCore::IsEnumType_v<TFlag> || JCore::IsIntegerType_v<TFlag>, "... TFlag must be enum or integer type");

	void Add(TFlag flag1, TFlag flag2 = (TFlag)0, TFlag flag3 = (TFlag)0, TFlag flag4 = (TFlag)0) {
		const int v = flag1 | flag2 | flag3 | flag4;
		Value |= v;
	}

	void Set(TFlag flag1, TFlag flag2 = (TFlag)0, TFlag flag3 = (TFlag)0, TFlag flag4 = (TFlag)0) {
		const int v = flag1 | flag2 | flag3 | flag4;
		Value = v;
	}

	void Unset(TFlag flag1, TFlag flag2 = (TFlag)0, TFlag flag3 = (TFlag)0, TFlag flag4 = (TFlag)0) {
		const int v = flag1 | flag2 | flag3 | flag4;
		Value &= ~v;
	}

	bool Check(TFlag flag1, TFlag flag2 = (TFlag)0, TFlag flag3 = (TFlag)0, TFlag flag4 = (TFlag)0) {
		const int v = flag1 | flag2 | flag3 | flag4;
		return (Value & v) == v;
	}

	void Clear() { Value = 0; }

	TThis& operator=(TFlag flag) { Value = flag; return *this; }
	TThis& operator|=(TFlag flag) { Value |= flag; return *this; }
	TThis& operator&=(TFlag flag) { Value &= flag; return *this; }
	TThis& operator^=(TFlag flag) { Value ^= flag; return *this; }

	TFlag operator|(TFlag flag) { return Value | flag; }
	TFlag operator&(TFlag flag) { return Value & flag; }
	TFlag operator^(TFlag flag) { return Value ^ flag; }

	bool operator==(TFlag flag) { return Value == flag; }
	bool operator!=(TFlag flag) { return Value != flag; }
	bool operator>(TFlag flag) { return Value > flag; }
	bool operator<(TFlag flag) { return Value < flag; }
	bool operator>=(TFlag flag) { return Value >= flag; }
	bool operator<=(TFlag flag) { return Value <= flag; }

	int Value;
};

NS_JC_END