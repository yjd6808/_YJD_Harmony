/*
	작성자 : 윤정도
	콜렉션 최상위 객체
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
												반복자 최상위 인터페이스
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
										 순회 가능한 = 컨테이너 최상위 인터페이스
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
										          콜렉션
										 모든 컨테이너의 기본 인터페이스
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
											다이나믹 배열
					        배열 스택, 배열 큐, 배열 리스트의 공통 인터페이스 정의
		 =====================================================================================*/

		
		template <typename T>
		class DynamicArray : public Collection<T>
		{
			static constexpr int ms_iExpandingFactor = 4;	// 꽉차면 4배씩 확장
			static constexpr int ms_iDefaultCapcity = 32;	// 초기 배열 크기

		public:
			DynamicArray(int capacity = ms_iDefaultCapcity) {
				m_pArray = new T[capacity];
				m_iCapacity = capacity;
			}

			virtual ~DynamicArray() {
				delete[] m_pArray;
			}
		protected:
			// 크기 확장
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
					throw OutOfRangeException("올바르지 않은 인덱스 입니다.");
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
											배열 스택
							배열 스택, 배열 큐, 배열 리스트의 공통 인터페이스 정의
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