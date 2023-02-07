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


#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>

#include <JCore/Pool/MemoryPoolStrategy.h>
#include <JCore/Pool/MemoryPoolAllocationAlgorithm.h>
#include <JCore/Pool/MemoryPoolAbstract.h>


namespace JCore {
	


	template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm>
	class MemoryPool {};

	using MemoryPoolSingleBinary	  = MemoryPool<eSingle, eBinarySearch>;
	using MemoryPoolSingleBinaryPtr   = SharedPtr<MemoryPoolSingleBinary>;
	using MemoryPoolMultipleBinary	  = MemoryPool<eMultiple, eBinarySearch>;
	using MemoryPoolMultipleBinaryPtr = SharedPtr<MemoryPoolMultipleBinary>;



	template <>
	class MemoryPool<eSingle, eBinarySearch> : 
		public MemoryPoolAbstract, 
		public MakeSharedFromThis<MemoryPoolSingleBinary>
	{
		using Type = MemoryPoolSingleBinary;
	public:
		MemoryPool(const HashMap<int, int>& allocationMap) : MemoryPoolAbstract(false) {
			Type::Initialize(allocationMap);
		}

		MemoryPool(bool skipInitialize) : MemoryPoolAbstract(skipInitialize) {}
		MemoryPool(int slot, const String& name, bool skipInitialize = false) : MemoryPoolAbstract(slot, name, skipInitialize) {}
		~MemoryPool() override {
			auto dbg = this;
			Type::Finalize();
		}

		template <int RequestSize>
		void* StaticPop()  {
			constexpr int iIndex = Detail::AllocationLengthMapConverter::ToIndex<RequestSize>();
			constexpr int iFitSize = Detail::AllocationLengthMapConverter::ToSize<iIndex>();

			void* pMemoryBlock = nullptr;
			{
				LockGuard<NormalLock> guard(m_Lock);

				if (m_Pool[iIndex].IsEmpty()) {
					AddAllocated(iIndex, true);
					return Memory::Allocate<void*>(iFitSize);
				}

				pMemoryBlock = m_Pool[iIndex].Top();
				m_Pool[iIndex].Pop();
			}
			AddAllocated(iIndex, false);
			return pMemoryBlock;
		}

		void* DynamicPop(int requestSize, int& realAllocatedSize) override {
			int iIndex = Detail::AllocationLengthMapConverter::ToIndex(requestSize);
			int iFitSize = Detail::AllocationLengthMapConverter::ToSize(iIndex);

			realAllocatedSize = iFitSize;
			void* pMemoryBlock = nullptr;

			{
				LockGuard<NormalLock> guard(m_Lock);

				if (m_Pool[iIndex].IsEmpty()) {
					AddAllocated(iIndex, true);
					return Memory::Allocate<void*>(iFitSize);
				}

				pMemoryBlock = m_Pool[iIndex].Top();
				m_Pool[iIndex].Pop();
			}
			AddAllocated(iIndex, false);
			return pMemoryBlock;
		}

		template <int PushSize>
		void StaticPush(void* memory) {
			// static_assert(Detail::AllocationLengthMapConverter::ValidateSize<PushSize>());
			int index = Detail::AllocationLengthMapConverter::ToIndex<PushSize>();
			LockGuard<NormalLock> guard(m_Lock);
			AddDeallocated(index);
			m_Pool[index].Push(memory);
		}

		void DynamicPush(void* memory, int returnSize) override {
			// DebugAssertMessage(Detail::AllocationLengthMapConverter::ValidateSize(returnSize), "뭐야! 사이즈가 안맞자나!");
			int index = Detail::AllocationLengthMapConverter::ToIndex(returnSize);
			LockGuard<NormalLock> guard(m_Lock);
			AddDeallocated(index);
			m_Pool[index].Push(memory);
		}


		void Initialize(const HashMap<int, int>& allocationMap) override {
			const_cast<HashMap<int, int>&>(allocationMap).Extension().ForEach([this](Pair<int, int>& count) {
				int iSize = count.Key;
				int iCount = count.Value;
				int iIndex = Detail::AllocationLengthMapConverter::ToIndex(iSize);
				DebugAssertMsg(Detail::AllocationLengthMapConverter::ValidateSize(iSize), "뭐야! 사이즈가 안맞자나!");

				for (int i = 0; i < iCount; ++i) {
					m_Pool[iIndex].Push(Memory::Allocate<void*>(iSize));
				}

				AddInitBlock(iIndex, iCount);
			});

			m_bInitialized = true;
		}

		// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
		void Finalize() override {
			DebugAssertMsg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

			for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i) {
				while (!m_Pool[i].IsEmpty()) {
					Memory::Deallocate(m_Pool[i].Top());
					m_Pool[i].Pop();
				}
			}
		}

		int Strategy() override { return eSingle; }
		int Algorithm() override { return eBinarySearch; }
		
	private:
		ArrayStack<void*> m_Pool[Detail::MemoryBlockSizeMapSize_v];
		NormalLock m_Lock;
	};


};

