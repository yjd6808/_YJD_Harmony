/*
	�ۼ��� : ������
	�ݷ��� �ֻ��� ��ü
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/PointerObserver.h>
#include <JCore/SmartPointer.h>
#include <JCore/Memory.h>

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
			virtual T& Current() const = 0;

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
				while (it->HasNext()) {
					fn(it->Current());
				}
			}

		public:
			inline SharedPointer<TCollection> ToCollection() = 0;
			inline bool IsEmpty() const { return m_iSize != 0; }
			inline int GetSize() const { return m_iSize; }
		protected:
			int m_iSize = 0;
		};


		/*=====================================================================================
											���̳��� �迭
					        �迭 ����, �迭 ť, �迭 ����Ʈ�� ���� �������̽� ����
		 =====================================================================================*/

		
		template <typename T>
		class DynamicArray : public Collection<T>
		{
			static constexpr int ms_iExpandingFactor = 4;	// ������ 4�辿 Ȯ��
			static constexpr int ms_iDefaultCapcity = 32;	// �ʱ� �迭 ũ��

		public:
			DynamicArray(int capacity = ms_iDefaultCapcity) {
				m_pArray = new T[capacity];
				m_iCapacity = capacity;
			}

			virtual ~DynamicArray() {
				delete[] m_pArray;
			}
		protected:
			// ũ�� Ȯ��
			void Expand(int capacity) {
				T* pNewArray = new T[capacity];
				Memory::Copy(pNewArray, sizeof(T) * capacity, m_pArray, sizeof(T) * this->m_iSize);
				delete[] m_pArray;
				m_pArray = pNewArray;
			}


			T& GetAtUnsafe(const int idx) const {
				return m_pArray[idx];
			}

			T& GetAt(const int idx) {
				if (!IsValidRange(idx)) {
					throw OutOfRangeException("�ùٸ��� ���� �ε��� �Դϴ�.");
				}

				return m_pArray[idx];
			}

			T& operator[](const int idx) const {
				return GetAt(idx);
			}

			bool IsValidRange(const int idx) const {
				return idx >= 0 && idx < this->m_iSize;
			}

		private:
			T* m_pArray;
			int m_iCapacity;
		};



		template <typename T>
		class ArrayIterator : Iterator<T>
		{
		public:



			bool IsValid() const override {

			}

		protected:
			T* m_pArray;
			int m_iPos;
			//m_Watcher;

			friend class DynamicArray<T>;
		};


		/*=====================================================================================
											�迭 ����
							�迭 ����, �迭 ť, �迭 ����Ʈ�� ���� �������̽� ����
		 =====================================================================================*/

		//template <typename T>
		//class ArrayStack : DynamicArray<T>
		//{
		//	using TIterator		= typename Iterator<T>;
		//	using TDynamicArray = typename DynamicArray<T>;
		//public:
		//	ArrayStack(int capacity) : TDynamicArray(capacity) {

		//	}

		//	virtual ~ArrayStack() {}

		//	SharedPointer<TIterator> Begin() const {
		//		return MakeShared();
		//	}
		//};


	} // namespace Container
} // namespace JCore