/*
 * 작성자: 윤정도
 * 생성일: 12/12/2022 3:17:05 PM
 * =====================
 * 메모리 풀 종류가 많으므로 이를 관리해줄 매니저를 둔다.
 */


#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Pool/MemoryPool.h>

#define MemoryPoolTemplate		template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm>
#define MemoryPoolTemplateSlot	template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm, Int32 SlotIndex>
#define MemoryPoolTemplateType	MemoryPool<Strategy, Algorithm>

namespace JCore {
	namespace Detail {
		constexpr int MaxSlot_v = 32;
		constexpr int ArraySlot_v = 0;
		constexpr int ListSlot_v = 1;
		constexpr int SmartPtrSlot_v = 2;
	}

	class MemoryPoolManager
	{
	public:
		MemoryPoolManager() = default;
	public:
		MemoryPoolTemplate
			void Register(int slot, const String& name, bool skipInitialize = false) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(slot < Detail::MaxSlot_v&& m_pPool[iPoolIndex][slot].Get() == nullptr,
				"해당 슬롯에 이미 메모리풀이 등록되어 있습니다.");

			auto pNewRegisteredPool = MakeShared<MemoryPoolTemplateType>(slot, name, skipInitialize);
			m_pPool[iPoolIndex][slot] = pNewRegisteredPool;
			m_pRegisteredPool.PushBack(pNewRegisteredPool);
		}

		MemoryPoolTemplate
			void Initialize(int slot, const JCore::HashMap<int, int>& Counters) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][slot].Get() != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			DebugAssertMessage(m_pPool[iPoolIndex][slot]->IsInitialized() == false, "이미 초기화가 진행되었습니다.");
			m_pPool[iPoolIndex][slot]->Initialize(Counters);
		}

		MemoryPoolTemplate
		MemoryPoolAbstractPtr Get(int slot) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][slot].Get() != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			return m_pPool[iPoolIndex][slot];
		}


		void FinalizeAll() {
			/* 따로 그냥 연결리스트를 만들어야겠다.
			for (int iStrategyIndex = eSingle; iStrategyIndex <= eMemoryPoolStrategyMax; iStrategyIndex += 0x10) {
				for (int iAlgorithmIndex = eBinarySearch; iAlgorithmIndex <= eMemoryPoolAllocationAlgorithmMax; iAlgorithmIndex += 1) {
					for (int i = 0; i < Detail::MaxSlot_v; ++i) {
						if (m_pPool[iStrategyIndex | iAlgorithmIndex][i]) {
							m_pPool[iStrategyIndex | iAlgorithmIndex][i]->Finalize();
							DeleteSafe(m_pPool[iStrategyIndex | iAlgorithmIndex][i]);
						}
					}
				}
			}
			*/

			m_pRegisteredPool.Extension().ForEach([this](MemoryPoolAbstractPtr& pool) {
				int iPoolIndex = pool->Strategy() | pool->Algorithm();
				int iSlot = pool->Slot();

				m_pPool[iPoolIndex][iSlot]->Finalize();
				m_pPool[iPoolIndex][iSlot] = nullptr;
			});
		}

		bool IsFinalized() {
			/* 따로 그냥 연결리스트를 만들어야겠다.
			for (int iStrategyIndex = eSingle; iStrategyIndex <= eMemoryPoolStrategyMax; iStrategyIndex += 0x10) {
				for (int iAlgorithmIndex = eBinarySearch; iAlgorithmIndex <= eMemoryPoolAllocationAlgorithmMax; iAlgorithmIndex += 1) {
					for (int i = 0; i < Detail::MaxSlot_v; ++i) {
						if (m_pPool[iStrategyIndex | iAlgorithmIndex][i]) {
							return false;
						}
					}
				}
			}
			*/
			return m_pRegisteredPool.IsEmpty();
		}

		void StartDetectLeak() {
			auto pool = m_pRegisteredPool.Extension().FindIf([](MemoryPoolAbstractPtr& pool) { return pool->Detecting(); });
			DebugAssertMessage(pool == nullptr, "현재 메모리릭을 검사중인 풀이 있습니다!");
			m_pRegisteredPool.Extension().ForEach([](MemoryPoolAbstractPtr& pool) { pool->StartDetectLeak(); });
			m_bDetecting = true;
		}

		/**
		 * \param leakedPools 각 풀마다 릭이 얼마나 있는지 확인하고자할 때
		 * \return 전체 메모리풀의 메모리릭 크기
		 */
		Int64U StopDetectLeak(OutOpt_ LinkedList<MemoryPoolCapturedPtr>* leakedPools = nullptr) {
			DebugAssertMessage(Detecting(), "어라? StartDetectLeak()이 호출되지 않았어요.");
			Int64U uiTotalLeakedBytes = 0;
			m_pRegisteredPool.Extension().ForEach([&uiTotalLeakedBytes, &leakedPools](MemoryPoolAbstractPtr& pool) {
				auto spMemPoolCaptured = MakeShared<MemoryPoolCaptured>();
				Int64U uiLeakedBytes = pool->StopDetectLeak(spMemPoolCaptured->LeakBlocks);
				uiTotalLeakedBytes += uiLeakedBytes;
				if (uiLeakedBytes > 0 && leakedPools) {
					leakedPools->PushBack(spMemPoolCaptured);
				}
			});

			return uiTotalLeakedBytes;
		}

		bool Detecting() { return m_bDetecting; }

	private:
		MemoryPoolAbstractPtr m_pPool[eMemoryPoolStrategyMax | eMemoryPoolAllocationAlgorithmMax][Detail::MaxSlot_v]{};
		LinkedList<MemoryPoolAbstractPtr> m_pRegisteredPool;
		bool m_bDetecting{};
	};
} // namespace JCore


#undef MemoryPoolTemplate
#undef MemoryPoolTemplateSlot
#undef MemoryPoolTemplateType

