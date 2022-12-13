/*
 * 작성자: 윤정도
 * 생성일: 12/12/2022 3:17:05 PM
 * =====================
 * 메모리 풀 종류가 많으므로 이를 관리해줄 매니저를 둔다.
 */


#pragma once

#include <JCore/Container/Vector.h>
#include <JCore/Pool/MemoryPool.h>

#define MemoryPoolTemplate		template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm>
#define MemoryPoolTemplateSlot	template <MemoryPoolStrategy Strategy, MemoryPoolAllocationAlgorithm Algorithm, Int32 SlotIndex>
#define MemoryPoolTemplateType	MemoryPool<Strategy, Algorithm>

namespace JCore {
	namespace Detail {
		constexpr int MaxSlot_v = 32;
	}

	class MemoryPoolManager
	{
	public:
		MemoryPoolTemplateSlot
		void Register(bool skipInitialize = false) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(SlotIndex < Detail::MaxSlot_v && m_pPool[iPoolIndex][SlotIndex] == nullptr, 
				"해당 슬롯에 이미 메모리풀이 등록되어 있습니다.");
			m_pPool[iPoolIndex][SlotIndex] = new MemoryPoolTemplateType;
			m_bInitialized[iPoolIndex][SlotIndex] = skipInitialize;
		}

		MemoryPoolTemplateSlot
		void Initialize(const JCore::HashMap<int, int>& Counters) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][SlotIndex] != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			DebugAssertMessage(m_bInitialized[iPoolIndex][SlotIndex] == false, "이미 초기화가 진행되었습니다.");
			m_pPool[iPoolIndex][SlotIndex]->Initialize(Counters);
			m_bInitialized[iPoolIndex][SlotIndex] = true;
		}

		MemoryPoolTemplateSlot
		MemoryPoolTemplateType* Get() {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][SlotIndex] != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			return reinterpret_cast<MemoryPoolTemplateType*>(m_pPool[iPoolIndex][SlotIndex]);
		}

		MemoryPoolTemplate
			void Register(int slot, bool skipInitialize = false) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(slot < Detail::MaxSlot_v&& m_pPool[iPoolIndex][slot] == nullptr,
				"해당 슬롯에 이미 메모리풀이 등록되어 있습니다.");
			m_pPool[iPoolIndex][slot] = new MemoryPoolTemplateType;
			m_bInitialized[iPoolIndex][slot] = skipInitialize;
		}

		MemoryPoolTemplate
			void Initialize(int slot, const JCore::HashMap<int, int>& Counters) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][slot] != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			DebugAssertMessage(m_bInitialized[iPoolIndex][slot] == false, "이미 초기화가 진행되었습니다.");
			m_pPool[iPoolIndex][slot]->Initialize(Counters);
			m_bInitialized[iPoolIndex][slot] = true;
		}

		MemoryPoolTemplate
			MemoryPoolTemplateType* Get(int slot) {
			constexpr int iPoolIndex = Strategy | Algorithm;
			DebugAssertMessage(m_pPool[iPoolIndex][slot] != nullptr, "먼저 해당슬롯에 메모리 풀을 등록해주세요.");
			return reinterpret_cast<MemoryPoolTemplateType*>(m_pPool[iPoolIndex][slot]);
		}


		void FinalizeAll() {
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
		}
	private:
		IMemoryPool* m_pPool[eMemoryPoolStrategyMax | eMemoryPoolAllocationAlgorithmMax][Detail::MaxSlot_v]{};
		bool m_bInitialized[eMemoryPoolStrategyMax | eMemoryPoolAllocationAlgorithmMax][Detail::MaxSlot_v]{};
	};
} // namespace JCore


inline JCore::MemoryPoolManager g_DefaultMemoryPoolManager;

#undef MemoryPoolTemplate
#undef MemoryPoolTemplateType

