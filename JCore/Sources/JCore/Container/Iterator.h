/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/PointerObserver.h>

namespace JCore {

/*=====================================================================================
						�ݺ��� �ֻ��� �������̽�
=====================================================================================*/

// ���� ����
template <typename> class Collection;
template <typename T>
struct Iterator
{
	using TCollection = typename Collection<T>;

	Iterator(VoidOwner& owner) : Watcher(owner) {}
	virtual ~Iterator() noexcept { Watcher.~VoidWatcher(); }

	virtual bool HasValue() const = 0;

	virtual T& Previous() = 0;
	virtual T& Next() = 0;

	virtual bool IsValid() const {
		return Watcher.Exist();
	}

protected:
	TCollection* CastCollection() const {
		ThrowIfIteratorIsNotValid();
		return Watcher.Get<TCollection>();
	}

	void ThrowIfIteratorIsNotValid() const {
		if (!IsValid()) {
			throw NullPointerException("�ش� ���ͷ����ʹ� ���̻� �������� �ʽ��ϴ�.");
		}
	}
protected:
	union { VoidWatcher Watcher; };
};

} // namespace JCore