/*
	�ۼ��� : ������
	�Ҹ��� ȣ��� �۾��� ó���ϱ� ���� �뵵
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