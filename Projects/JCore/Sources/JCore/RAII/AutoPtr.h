/*
 *	작성자 : 윤정도
 *	소멸자 호출로 작업을 처리하기 위한 용도
 */

#pragma once

#include <JCore/Functional.h>
#include <JCore/TypeCast.h>

NS_JC_BEGIN

template <typename T, typename TAction = Action<T*>>
class AutoPtr
{
public:
	AutoPtr(T* ptr) : m_pPtr(ptr) {}
	AutoPtr(T* ptr, TAction&& fn) : m_pPtr(ptr), m_Fn(Move(fn)) {}
	~AutoPtr() { m_Fn(m_pPtr); }
private:
	T* m_pPtr;
	TAction m_Fn;
};

template <typename T, typename TAction>
class AutoPtr<T&, TAction> {};



NS_JC_END