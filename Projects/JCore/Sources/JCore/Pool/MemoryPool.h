/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 12:01:05 AM
 * =====================
 * 우선 락프리 컨테이너를 구현하기전에는 뮤텍스로 원자적 수행을 보장토록 해준다.
 * 메모리풀 로직을 어떻게 구현할까?
 *
 * 1. 53바이트를 요청하면 64바이트를 줘야하는데.
 *    이걸 엄청 빠르게 런타임에 계산하는 방법이 1 ~ 1 << 29사이를 이분탐색으로 확인하는 방법
 *    ==> BinarySearch으로 명명함
 *
 * 2. 혹은 보통 메모리 할당요청하는게 보통 1만 바이트이하니까
 *    ArrayStack<void*>* m_Pool[4096]로 잡아서
 *	  ArrayStack<void*> pool[13];
 *	  m_Pool[0 ~ 1]  -> &pool[0] // 1바이트 풀
 *	  m_Pool[2]		 -> &pool[1] // 2바이트 풀
 *	  m_Pool[3 ~ 4]  -> &pool[2] // 4바이트 풀
 *	  m_Pool[5 ~ 7]  -> &pool[3] // 8바이트 풀
 *	  m_Pool[8 ~ 15] -> &pool[4] // 16바이트 풀
 *	  이런식으로 O(1)의 속도로 Push/Pop 가능하게 만들어주고
 *
 *	  그리고 8192바이트부터는 VirtualAlloc을 사용하고..
 *	  이때부터는 단위를 1만으로 나누는거지.
 *	  1 << 13(8'192) ~ 1 << 24(16'777'216)
 *	  0 ~ 16'777 대략 0.06MB 크기의 int 배열 하나만 있으면 1600만 바이트까지 O(1)의 속도로 할당가능
 *	     ==> FullIndexing으로 명명함
 *
 * 3. 고정 사이즈를 할당해주는 효율적인 방법은 없을까?
 *    이건 나중에 고민하는걸로
 *
 *
 * ======================================================================
 * TODO: 락프리 컨테이너로 변경
 * TODO: 2번 방식으로 구현 변경
 */


#pragma once

#include <JCore/Limit.h>
#include <JCore/Sync/NormalLock.h>

#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>

#include <JCore/Pool/IMemoryPool.h>
#include <JCore/Pool/MemoryPoolStrategy.h>
#include <JCore/Pool/MemoryPoolAllocationAlgorithm.h>


namespace JCore {
	namespace Detail {

		// 53바이트, 73바이트 뭐 이런식으로 정밀하게 할당해줄 수 없기때문에
		// 53바이트를 요청하면 64바이트를
		// 74바이트를 요청하면 128바이트를 할당해주는 식으로 처리

		// C는 컴파일타임에 사용하는 용도
		// R은 런타임에 사용하는 용도

		// 1, 2, 4, 8, 16 ... 8'388'608, 16'777'216
		inline constexpr int CAllocationLengthMap_v[]{
			1 << 0,  1 << 1,  1 << 2,  1 << 3,  1 << 4,
			1 << 5,  1 << 6,  1 << 7,  1 << 8,  1 << 9,
			1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14,
			1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19,
			1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24
		};

		inline const int RAllocationLengthMap_v[]{
			1 << 0,  1 << 1,  1 << 2,  1 << 3,  1 << 4,
			1 << 5,  1 << 6,  1 << 7,  1 << 8,  1 << 9,
			1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14,
			1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19,
			1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24
		};

		inline constexpr int AllocationLegthMapSize_v = sizeof(CAllocationLengthMap_v) / sizeof(int);

		// 예를들어서
		// 64바이트 -> 6으로 변환
		//  6을    -> 64바이트로 변환 해주는 기능 수행
		struct AllocationLengthMapConverter {
			template <Int32 Size, Int32 Index = 0>
			static constexpr int ToIndex() {
				static_assert(Index >= 0 && Index < AllocationLegthMapSize_v, "... cannot find valid index [AllocationLengthMapConverter]");
				if constexpr (Size <= CAllocationLengthMap_v[Index])
					return Index;
				else
					return ToIndex<Size, Index + 1>();
			}


			template <Int32 Index>
			static constexpr int ToSize() {
				static_assert(Index >= 0 && Index < AllocationLegthMapSize_v, "... cannot find valid size [AllocationLengthMapConverter]");
				return CAllocationLengthMap_v[Index];
			}

			static int ToIndex(Int32 size) {
				int iIndex = Arrays::LowerBound(Detail::RAllocationLengthMap_v, size);
				DebugAssertMessage(iIndex >= 0 && iIndex < AllocationLegthMapSize_v, "전달한 Size로 할당가능한 사이즈에 맞는 풀이 없어요");
				return iIndex;
			}

			static int ToSize(Int32 index) {
				DebugAssertMessage(index >= 0 && index < AllocationLegthMapSize_v, "전달한 Index에 해당하는 풀이 없어요");
				return Detail::CAllocationLengthMap_v[index];
			}
		};
	}



	template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm>
	class MemoryPool {};

	template<>
	class MemoryPool<eSingle, eBinarySearch> : public IMemoryPool
	{
	private:
		MemoryPool() = default;
	public:
		template <int PopSize>
		void* StaticPop()  {
			constexpr int iIndex = Detail::AllocationLengthMapConverter::ToIndex<PopSize>();
			constexpr int iFitSize = Detail::AllocationLengthMapConverter::ToSize<iIndex>();

			void* pMemoryBlock = nullptr;
			{
				LockGuard<NormalLock> guard(m_Lock);

				if (m_Pool[iIndex].IsEmpty()) {
					return Memory::Allocate<void*>(iFitSize);
				}

				pMemoryBlock = m_Pool[iIndex].Top();
				m_Pool[iIndex].Pop();
			}
			return pMemoryBlock;
		}

		void* DynamicPop(int size) override {
			int iIndex = Detail::AllocationLengthMapConverter::ToIndex(size);
			void* pMemoryBlock = nullptr;
			{
				LockGuard<NormalLock> guard(m_Lock);

				if (m_Pool[iIndex].IsEmpty()) {
					int iFitSize = Detail::AllocationLengthMapConverter::ToSize(iIndex);
					return Memory::Allocate<void*>(iFitSize);
				}

				pMemoryBlock = m_Pool[iIndex].Top();
				m_Pool[iIndex].Pop();
			}
			return pMemoryBlock;
		}

		template <int PushSize>
		void StaticPush(void* memory) {
			int index = Detail::AllocationLengthMapConverter::ToIndex<PushSize>();
			LockGuard<NormalLock> guard(m_Lock);
			m_Pool[index].Push(memory);
		}

		void DynamicPush(void* memory, int size) override {
			int index = Detail::AllocationLengthMapConverter::ToIndex(size);
			LockGuard<NormalLock> guard(m_Lock);
			m_Pool[index].Push(memory);
		}

		void Initialize(const HashMap<int, int>& Counter) override {
			const_cast<HashMap<int, int>&>(Counter).Extension().ForEach([this](KeyValuePair<int, int>& count) {
				int iSize = count.Key;
				int iCount = count.Value;
				int iIndex = Detail::AllocationLengthMapConverter::ToIndex(iSize);

				for (int i = 0; i < iCount; ++i) {
					m_Pool[iIndex].Push(Memory::Allocate<void*>(iSize));
				}
			});
		}

		// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
		void Finalize() override {
			for (int i = 0; i < Detail::AllocationLegthMapSize_v; ++i) {
				while (!m_Pool[i].IsEmpty()) {
					Memory::Deallocate(m_Pool[i].Top());
					m_Pool[i].Pop();
				}
			}
		}
	private:
		ArrayStack<void*> m_Pool[Detail::AllocationLegthMapSize_v];
		NormalLock m_Lock;

		friend class MemoryPoolManager;
	};
};

