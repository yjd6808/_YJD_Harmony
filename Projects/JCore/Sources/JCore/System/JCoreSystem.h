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
	
	template <>
	class System<SystemName_v> : public SystemAbstract
	{
	public:
		System();
		~System() override;

		void EnableLeakCheck(bool enable);
		void OnStartUp() override;
		void OnTerminate() override;
		void RegisterPrioritySingleton(IPrioritySingleton*) override;
	private:
		void InitializeMemoryPool();
		void DestroyMemoryPool();
	private:
		bool m_bLeakCheckEnabled;
	};

	// 시스템 초기화 함수
	// main 함수에서 호출해줘야 정상적으로 PrioritySingle의 생성순서대로 오브젝트가 생성된다.
	// 이걸 없앨 수 있는 방법 구상
	void Initialize();

	using JCoreSystem = System<SystemName_v>;
	inline JCoreSystem JCoreSystem_v;

	inline MemoryPoolManager JCoreMemPoolManager_v;
	inline MemoryPool<eSingle, eBinarySearch>* JCoreArrayAllocatorPool_v = nullptr;
	inline MemoryPool<eSingle, eBinarySearch>* JCoreListAllocatorPool_v = nullptr;
	inline MemoryPool<eSingle, eBinarySearch>* JCoreSmartPtrAllocatorPool_v = nullptr;

} // namespace JCore


