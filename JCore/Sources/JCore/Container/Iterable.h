/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/SmartPointer.h>

namespace JCore {

/*=====================================================================================
						��ȸ ������ = �����̳� �ֻ��� �������̽�
=====================================================================================*/

// ���� ����
template <typename> class Iterator;
template <typename T>
struct Iterable
{
	using TIterator = typename Iterator<T>;

	Iterable() {}
	virtual ~Iterable() noexcept {}

	virtual SharedPointer<TIterator> Begin() const = 0;
	virtual SharedPointer<TIterator> End() const = 0;
};

// ����Ʈ ������ ���Ⱑ ����Ƿ�...
template <typename T>
using Enumerator = SharedPointer<Iterator<T>>;


} // namespace JCore