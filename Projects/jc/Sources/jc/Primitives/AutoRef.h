/*
 *	작성자 : 윤정도
 *	소멸자 호출로 작업을 처리하기 위한 용도
 */

#pragma once

#include <jc/Functional.h>
#include <jc/TypeCast.h>

NS_JC_BEGIN

template <typename T, typename TAction = Action<T>>
class AutoRef
{
public:
	AutoRef(T& _ref) : ref_(_ref) {}
	AutoRef(T& _ref, TAction&& _fn) : ref_(_ref), fn_(Move(_fn)) {}
	~AutoRef() { fn_(ref_); }
private:
	T& ref_;
	TAction fn_;
};

template <typename T, typename TAction>
class AutoRef<T*, TAction>
{
};

NS_JC_END
