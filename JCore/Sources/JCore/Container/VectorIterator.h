/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/DynamicArrayIterator.h>

namespace JCore {

// ���� ����
					class VoidOwner;
template <typename> class Vector;
template <typename T>
class VectorIterator : public DynamicArrayIterator<T>
{
	using TDynamicArrayIterator = typename DynamicArrayIterator<T>;
	using TVector				= typename Vector<T>;
public:
	VectorIterator(VoidOwner& owner, int pos) : TDynamicArrayIterator(owner, pos) {}
	virtual ~VectorIterator() noexcept {}
public:
	virtual bool HasValue() const {
		return TDynamicArrayIterator::HasValue();
	}

	virtual T& Next() {
		return TDynamicArrayIterator::Next();
	}

	virtual T& Previous() {
		return TDynamicArrayIterator::Previous();
	}
};

} // namespace JCore