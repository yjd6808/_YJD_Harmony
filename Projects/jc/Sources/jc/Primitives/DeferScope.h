/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 스코프 종료 시 지정된 작업을 수행하기 위한 용도
 */


#pragma once

#include "jc/Namespace.h"
#include "jc/TypeCast.h"

NS_JC_BEGIN

template <typename TFunc>
class ScopeExit
{
public:
	explicit ScopeExit(TFunc&& _func)
		: func_(Forward<TFunc>(_func))
		, active_(true)
	{
	}

	ScopeExit(const ScopeExit& _other) = delete;
	ScopeExit& operator=(const ScopeExit& _other) = delete;

	ScopeExit(ScopeExit&& _other) noexcept
		: func_(Move(_other.func_))
		, active_(_other.active_)
	{
		_other.Dismiss();
	}

	ScopeExit& operator=(ScopeExit&& _other) = delete;

	~ScopeExit()
	{
		if (active_)
			func_();
	}

	void Dismiss()
	{
		active_ = false;
	}

private:
	TFunc func_;
	bool active_;
};

NS_DETAIL_BEGIN

struct ScopeExitHelper
{
	template <typename TFunc>
	ScopeExit<TFunc> operator+(TFunc&& _func) const
	{
		return ScopeExit<TFunc>(Forward<TFunc>(_func));
	}
};

NS_END

NS_END
