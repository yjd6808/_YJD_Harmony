/*
	�ۼ��� : ������
	�ݷ��� �ֻ��� ��ü
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/PointerObserver.h>
#include <JCore/SmartPointer.h>

#include <functional>

namespace JCore {
	namespace Container {

		enum class ContainerType {
			ArrayQueue,
			ArrayStack,
			Vector,
			ListQueue,
			ListStack,
			LinkedList,
			HashMap,
			TreeMap
		};

		/*=====================================================================================
												�ݺ��� �ֻ��� �������̽�
		 =====================================================================================*/

		template <typename T>
		struct Iterator
		{
			virtual bool HasPrevious() const = 0;
			virtual bool HasNext() const = 0;

			virtual T& Previous() const = 0;
			virtual T& Next() const = 0;

			virtual bool IsValid() const = 0;
		};

		


		/*=====================================================================================
										 ��ȸ ������ = �����̳� �ֻ��� �������̽�
		 =====================================================================================*/

		template <typename T>
		struct Iterable
		{
			using TIterator = typename Iterator<T>;

					 Iterable() {}
			virtual ~Iterable() {}

			virtual SharedPointer<TIterator> Begin() const = 0;
			virtual SharedPointer<TIterator> End() const = 0;
		};

		/*=====================================================================================
										          �ݷ���
										 ��� �����̳��� �⺻ �������̽�
		 =====================================================================================*/

		template <typename T>
		class Collection : public Iterable<T>
		{
			using TCollection	= typename Collection<T>;
			using TIterator		= typename Iterator<T>;
		public:
			Collection() { }
			virtual ~Collection() {}
		public:
			void ForEach(std::function<void(T&)> fn) {
				auto it = this->Begin();
			}

		public:
			inline virtual SharedPointer<TCollection> ToCollection() = 0;
			inline virtual bool IsEmpty() const { return m_Size != 0; }
			inline virtual int GetSize() const { return m_Size; }
		protected:
			int m_Size = 0;
		};
	} // namespace Container
} // namespace JCore