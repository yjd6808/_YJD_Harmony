/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/SmartPointer.h>
#include <JCore/Declspec.h>

namespace JCore {

/*=====================================================================================
						��ȸ ������ = �����̳� �ֻ��� �������̽�
=====================================================================================*/

// ���� ����
template <typename> struct Iterator;
template <typename T>
struct JCORE_NOVTABLE Iterable
{
	using TIterator = Iterator<T>;

	Iterable() {}
	virtual ~Iterable() noexcept {}

	virtual SharedPointer<TIterator> Begin() const = 0;
	virtual SharedPointer<TIterator> End() const = 0;
};

// ����Ʈ ������ ���Ⱑ ����Ƿ�...
template <typename T>
using Enumerator = SharedPointer<Iterator<T>>;


} // namespace JCore