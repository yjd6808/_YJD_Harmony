/*
 *	작성자 : 윤정도
 *	소멸자 호출로 작업을 처리하기 위한 용도
 */

#pragma once

#include <jc/Functional.h>
#include <jc/TypeCast.h>

NS_JC_BEGIN

template <typename T, typename TAction = Action<T*>>
class AutoPtr
{
public:
	AutoPtr(T* _pPtr)
		: ptr_(_pPtr)
	{
	}

	AutoPtr(T* _pPtr, TAction&& _fn)
		: ptr_(_pPtr)
		, fn_(Move(_fn))
	{
	}

	~AutoPtr()
	{
		fn_(ptr_);
	}

private:
	T* ptr_;
	TAction fn_;
};

template <typename T, typename TAction>
class AutoPtr<T&, TAction>
{
};

NS_JC_END
