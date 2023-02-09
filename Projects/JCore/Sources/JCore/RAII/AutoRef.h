/*
 *	작성자 : 윤정도
 *	소멸자 호출로 작업을 처리하기 위한 용도
 */

#pragma once

#include <JCore/Functional.h>
#include <JCore/TypeCast.h>

NS_JC_BEGIN

template <typename T, typename TAction = Action<T>>
class AutoRef
{
public:
	AutoRef(T& ref) : m_Ref(ref) {}
	AutoRef(T& ref, TAction&& fn) : m_Ref(ref), m_Fn(Move(fn)) {}
	~AutoRef() { m_Fn(m_Ref); }
private:
	T& m_Ref;
	TAction m_Fn;
};

template <typename T, typename TAction>
class AutoRef<T*, TAction>
{
};

NS_JC_END