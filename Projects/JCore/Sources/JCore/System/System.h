/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:26:07 PM
 * =====================
 * 라이브러리 자체의 전반적인 조작을 담당하는 클래스
 * 하위 라이브러리 혹은 애플리케이션에서 확장해서 구현하면 됨.
 *
 * [현재 기능]
 *		1. 글로벌 오브젝트 생성/소멸 순서 결정
 *	
 */


#pragma once


namespace JCore {
	struct IPrioritySingleton;
	namespace Detail {

		// 시스템에서 직접 생성/소멸을 관리하는 클래스들
		struct ManagedClassNode {
			IPrioritySingleton* Class{};
			ManagedClassNode* Next{};
		};
	}

	// 전 시스템 공통 기능
	struct SystemAbstract
	{
	public:
		virtual ~SystemAbstract();
	protected:
		virtual void OnStartUp();
		virtual void OnTerminate();
		virtual void RegisterPrioritySingleton(IPrioritySingleton* priorityClass) = 0;
		void ConstructGlobalObject();
		void DestroyGlobalObject();
		void CreateClassConstructionOrder(IPrioritySingleton* priorityClass);
		void CreateClassDestructionOrder(IPrioritySingleton* priorityClass);

		Detail::ManagedClassNode* m_pConstructionOrderHead{};
		Detail::ManagedClassNode* m_pConstructionOrderTail{};
		Detail::ManagedClassNode* m_pDestructionOrderHead{};
		Detail::ManagedClassNode* m_pDestructionOrderTail{};
	};

	template <const char* = nullptr>
	class System {};
	
} // namespace JCore

