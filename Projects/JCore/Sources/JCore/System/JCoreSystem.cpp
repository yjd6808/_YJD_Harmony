/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 9:08:07 PM
 * =====================
 */

#include <JCore/Core.h>
#include <JCore/Debug/MemoryLeakDetector.h>
#include <JCore/System/PrioritySingleton.h>
#include <JCore/System/JCoreSystem.h>
#include <JCore/Pool/MemoryPoolManager.h>
#include <JCore/Utils/Console.h>


namespace JCore {
	namespace Detail {
		// JCore전용 메모리풀 매니저 싱글톤 객체 생성
		struct JCoreMemoryPoolManager :
			MemoryPoolManager,
			PrioritySingleton<JCore::SystemCode_v, JCoreMemoryPoolManager, 0, 0> {};
	}
	/*
	 * JCore 자체적으로 사용하는 변수들 여기 등록
	 */
	MemoryLeakDetector LeakDetector_v;
	


	JCoreSystem::System(const int systemCode): SystemAbstract(systemCode) {}
	JCoreSystem::~System() {}

	void JCoreSystem::EnableLeakCheck(bool enabled) {
		DebugAssertMessage(m_bStarted == false, "JCore::Initialize를 호출하기전에 설정해주세요.");
		m_bLeakCheckEnabled = enabled;
	}

	void JCoreSystem::OnStartUp() {
		m_bStarted = true;
		if (m_bLeakCheckEnabled) LeakDetector_v.StartDetect();

		SystemAbstract::OnStartUp();		// 클래스 생성	┐
		InitializeMemoryPool();				// ┐ 			│
	}										// │			│
											// │ 페어 (1)	│ 페어 (2)
	void JCoreSystem::OnTerminate() {		// │			│
		DestroyMemoryPool();				// ┘			│
		SystemAbstract::OnTerminate();		// 클래스 정리

		if (LeakDetector_v.Detecting()) {
			int unfreedMem = LeakDetector_v.StopDetect();
#ifdef DebugMode
			NormalConsole::WriteLine("%d 바이트 메모리 누수", unfreedMem);
			DebugAssertMessage(unfreedMem <= 0, "메모리 누수가 있습니다. 덴져로스");
#else
			assert(false);
#endif
		}
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
		JCoreMemPoolManager_v->FinalizeAll();
	}
	

	void Initialize() {
		SystemMap_v[JCore::SystemCode_v]->OnStartUp();
	}

	void Finalize() {
		SystemMap_v[JCore::SystemCode_v]->OnTerminate();
	}
}

