/*
	�ۼ��� : ������
	�Ҹ��� ȣ��� �۾��� ó���ϱ� ���� �뵵
*/

#pragma once

#include <functional>

namespace JCore {

template <typename T, typename OperatorFunctor = std::function<void(T&)>>
class AutoRef
{
public:
	AutoRef(T& ref) : m_Ref(ref) {}
	AutoRef(T& ref, OperatorFunctor fn) : m_Ref(ref), m_Fn(fn) {}
	~AutoRef() { m_Fn(m_Ref); }
private:
	T& m_Ref;
	OperatorFunctor m_Fn;
};

template <typename T, typename OperatorFunctor = std::function<void(T*)>>
class AutoPointer
{
public:
	AutoPointer(T* ptr) : m_Pointer(ptr) {}
	AutoPointer(T* ptr, OperatorFunctor fn) : m_Pointer(ptr), m_Fn(fn) {}
	~AutoPointer() { m_Fn(m_Pointer); }
private:
	T* m_Pointer;
	OperatorFunctor m_Fn;
};

} // namespace JCore