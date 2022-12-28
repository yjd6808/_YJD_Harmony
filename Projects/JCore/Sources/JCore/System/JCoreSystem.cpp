/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 9:08:07 PM
 * =====================
 * [! WARNING !]이 파일은 어디서도 쓰이지 않는 헤더파일이다.
 * 이번에 구현한 PrioritySingleton 기능 구현을 위해 작성된 파일들인데 제약사항이 너무 커서 
 * 폐기처분한다.
 *
 * 1. main 함수 시작/끝에서 클래스 생성/소멸을 위해 - initialize, finalize를 필수적으로 수행해야함.
 *   이때문에 전역코드 레벨에서 원하는 기능을 구현하는데 자유도가 떨어진다.
 * 2. 전역변수를 포인터로 두지말자. 객체가 존재하지 않는 경우를 신경 써야한다니..
 *    올라가야할 산 정상은 까마득한데 어깨에 무거운 짐을 짊어진 느낌..
 *
 * 다음부턴 이렇게 만들지 않기 위해 박제한다.
 */

#include <JCore/Core.h>
#include <JCore/Debug/MemoryLeakDetector.h>
#include <JCore/System/PrioritySingleton.h>
#include <JCore/System/JCoreSystem.h>
#include <JCore/Pool/MemoryPoolManager.h>
#include <JCore/Utils/Console.h>

#if 0

namespace JCore {
	namespace Detail {
		// JCore전용 메모리풀 매니저 싱글톤 객체 생성
		struct JCoreMemoryPoolManager :
			MemoryPoolManager,
			PrioritySingleton<JCore::SystemCode_v, JCoreMemoryPoolManager, 0, 0> {};
	}

	JCoreSystem::System(const int systemCode): SystemAbstract(systemCode) {}
	JCoreSystem::~System() {}

	void JCoreSystem::OnStartUp() {
		m_bStarted = true;
		SystemAbstract::OnStartUp();		// 클래스 생성	┐
		InitializeMemoryPool();				// ┐ 			│
	}										// │			│
											// │ 페어 (1)	│ 페어 (2)
	void JCoreSystem::OnTerminate() {		// │			│
		DestroyMemoryPool();				// ┘			│
		SystemAbstract::OnTerminate();		// 클래스 정리
	}

	void JCoreSystem::InitializeMemoryPool() {
		JCoreMemPoolManager_v = &Detail::JCoreMemoryPoolManager::GetInstance();
		JCoreMemPoolManager_v->Register<eSingle, eBinarySearch>(Detail::ArraySlot_v, "배열기반 컨테이너 전용 기본 메모리풀");
		JCoreMemPoolManager_v->Register<eSingle, eBinarySearch>(Detail::ListSlot_v, "연결리스트기반 컨테이너 전용 기본 메모리풀");
		JCoreMemPoolManager_v->Register<eSingle, eBinarySearch>(Detail::SmartPtrSlot_v, "스마트포인터 전용 기본 메모리풀", true);

		JCoreMemPoolManager_v->Initialize<eSingle, eBinarySearch>(Detail::ArraySlot_v, {
			{ 4, 0 },
			{ 8, 0 },
			{ 16, 64 },
			{ 32, 128 },
			{ 64, 128 },
			{ 128, 128 },
			{ 256, 64 },
			{ 512, 64 },
			{ 1024, 32 },
			{ 2048, 32 },
			{ 4096, 16 },
			{ 8192, 16 },
			{ 8192 * 2, 8 },
			{ 8192 * 4, 8 }
			});
		
		JCoreMemPoolManager_v->Initialize<eSingle, eBinarySearch>(Detail::ListSlot_v, {
			{ 4, 0 },
			{ 8, 0 },
			{ 16, 0 },
			{ 32, 256 },
			{ 64, 256 },
			{ 128, 128 },
			{ 256, 128 },
			{ 512, 32 },
			{ 1024, 16 }
		});

		JCoreArrayAllocatorPool_v = JCoreMemPoolManager_v->Get<eSingle, eBinarySearch>(Detail::ArraySlot_v);
		JCoreListAllocatorPool_v = JCoreMemPoolManager_v->Get<eSingle, eBinarySearch>(Detail::ListSlot_v);
		JCoreSmartPtrAllocatorPool_v = JCoreMemPoolManager_v->Get<eSingle, eBinarySearch>(Detail::SmartPtrSlot_v);

		
	}

	void JCoreSystem::DestroyMemoryPool() {
		JCoreArrayAllocatorPool_v = nullptr;
		JCoreListAllocatorPool_v = nullptr;
		JCoreSmartPtrAllocatorPool_v = nullptr;

		JCoreMemPoolManager_v->FinalizeAll();
	}
	

	void Initialize() {
		SystemMap_v[JCore::SystemCode_v]->OnStartUp();
	}

	void Finalize() {
		SystemMap_v[JCore::SystemCode_v]->OnTerminate();
	}
}


#endif