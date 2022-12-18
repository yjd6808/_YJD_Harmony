/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:26:07 PM
 * =====================
 * 
 */


#pragma once


#include <JCore/System/System.h>
#include <JCore/Pool/MemoryPoolManager.h>


namespace JCore {
	inline const char SystemName_v[] = "JCore";
	inline const int SystemCode_v = 0;
	
	template <>
	class System<SystemName_v> : public SystemAbstract
	{
	public:
		System(const int systemCode);
		~System() override;

		void OnStartUp() override;
		void OnTerminate() override;
	private:
		void InitializeMemoryPool();
		void DestroyMemoryPool();
	private:
		bool m_bLeakCheckEnabled;
	};


	
	// 시스템 초기화 함수
	// main 함수에서 호출해줘야 정상적으로 PrioritySingle의 생성순서대로 오브젝트가 생성된다.
	void Initialize();

	// main 함수가 끝나기전 Finalize를 호출해줘야 PrioritySingle의 소멸순서대로 오브젝트가 소멸된다.
	void Finalize();

	using JCoreSystem = System<SystemName_v>;
	inline JCoreSystem JCoreSystem_v{SystemCode_v};
	inline MemoryPoolManager* JCoreMemPoolManager_v{};
	inline MemoryPoolSingleBinaryPtr JCoreArrayAllocatorPool_v{};
	inline MemoryPoolSingleBinaryPtr JCoreListAllocatorPool_v{};
	inline MemoryPoolSingleBinaryPtr JCoreSmartPtrAllocatorPool_v{};

} // namespace JCore


