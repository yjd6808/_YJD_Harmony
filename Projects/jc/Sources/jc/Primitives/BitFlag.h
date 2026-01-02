/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 9:29:26 AM
 * =====================
 * 비트 플래그를 쉽게 수행하기 위함.
 */


#pragma once

#include <jc/TypeTraits.h>

NS_JC_BEGIN

template <typename TFlag>
struct BitFlag
{
	using TThis = BitFlag<TFlag>;

	BitFlag()
		: Value{}
	{
	}

	BitFlag(int _flag)
		: Value{ _flag }
	{
	}

	BitFlag(EnableIf_t<IsEnumType_v<TFlag>, TFlag> _flag)
		: Value{ static_cast<int>(_flag) } // enum, enum class를 위한 생성자
	{
	}

	BitFlag(const TThis& _other)
	{
		Value = _other.Value;
	}

	static_assert(jc::IsEnumType_v<TFlag> || jc::IsIntegerType_v<TFlag>, "... TFlag must be enum or integer type");

	void Add(TFlag _flag1, TFlag _flag2 = (TFlag)0, TFlag _flag3 = (TFlag)0, TFlag _flag4 = (TFlag)0)
	{
		const int combinedFlags = static_cast<int>(_flag1 | _flag2 | _flag3 | _flag4);
		Value |= combinedFlags;
	}

	void Set(TFlag _flag1, TFlag _flag2 = (TFlag)0, TFlag _flag3 = (TFlag)0, TFlag _flag4 = (TFlag)0)
	{
		const int combinedFlags = static_cast<int>(_flag1 | _flag2 | _flag3 | _flag4);
		Value = combinedFlags;
	}

	void Unset(TFlag _flag1, TFlag _flag2 = (TFlag)0, TFlag _flag3 = (TFlag)0, TFlag _flag4 = (TFlag)0)
	{
		const int combinedFlags = static_cast<int>(_flag1 | _flag2 | _flag3 | _flag4);
		Value &= ~combinedFlags;
	}

	bool Check(TFlag _flag1, TFlag _flag2 = (TFlag)0, TFlag _flag3 = (TFlag)0, TFlag _flag4 = (TFlag)0)
	{
		const int combinedFlags = static_cast<int>(_flag1 | _flag2 | _flag3 | _flag4);
		return (Value & combinedFlags) == combinedFlags;
	}

	void Clear()
	{
		Value = 0;
	}

	TThis& operator=(TFlag _flag)
	{
		Value = static_cast<int>(_flag);
		return *this;
	}

	TThis& operator|=(TFlag _flag)
	{
		Value |= static_cast<int>(_flag);
		return *this;
	}

	TThis& operator&=(TFlag _flag)
	{
		Value &= static_cast<int>(_flag);
		return *this;
	}

	TThis& operator^=(TFlag _flag)
	{
		Value ^= static_cast<int>(_flag);
		return *this;
	}

	TFlag operator|(TFlag _flag)
	{
		return Value | static_cast<int>(_flag);
	}

	TFlag operator&(TFlag _flag)
	{
		return Value & static_cast<int>(_flag);
	}

	TFlag operator^(TFlag _flag)
	{
		return Value ^ static_cast<int>(_flag);
	}

	bool operator==(TFlag _flag)
	{
		return Value == static_cast<int>(_flag);
	}

	bool operator!=(TFlag _flag)
	{
		return Value != static_cast<int>(_flag);
	}

	bool operator>(TFlag _flag)
	{
		return Value > static_cast<int>(_flag);
	}

	bool operator<(TFlag _flag)
	{
		return Value < static_cast<int>(_flag);
	}

	bool operator>=(TFlag _flag)
	{
		return Value >= static_cast<int>(_flag);
	}

	bool operator<=(TFlag _flag)
	{
		return Value <= static_cast<int>(_flag);
	}

	int Value;
};

NS_JC_END
