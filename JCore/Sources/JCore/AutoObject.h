/*
	작성자 : 윤정도
	소멸자 호출로 작업을 처리하기 위한 용도
*/

#pragma once

#include <functional>

namespace JCore {

template <typename T, typename OperatorFunctor>
class AutoRef
{
public:
	AutoRef(T& ref) : m_Ref(ref) {}
	~AutoRef() { OperatorFunctor()(m_Ref); }
private:
	T& m_Ref;
};

template <typename T, typename OperatorFunctor>
class AutoPointer
{
public:
	AutoPointer(T* ptr) : m_Pointer(ptr) {}
	~AutoPointer() { OperatorFunctor()(m_Pointer); }
private:
	T* m_Pointer;
};

} // namespace JCore