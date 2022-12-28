/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:26:07 PM
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
 * ====================================================================
 * 라이브러리 자체의 전반적인 조작을 담당하는 클래스
 * 하위 라이브러리 혹은 애플리케이션에서 확장해서 구현하면 됨.
 *
 * [현재 기능]
 *		1. 글로벌 오브젝트 생성/소멸 순서 결정
 *	
 */


#pragma once

#if 0

namespace JCore {
	struct IPrioritySingleton;
	namespace Detail {

		// 시스템에서 직접 생성/소멸을 관리하는 클래스들
		struct ManagedClassNode {
			IPrioritySingleton* Class{};
			ManagedClassNode* Next{};
		};

		constexpr int MaxSystemCode_v = 64;
	}

	// 전 시스템 공통 기능
	struct SystemAbstract
	{
	public:
		SystemAbstract(int code);
		virtual ~SystemAbstract();
		void RegisterPrioritySingleton(IPrioritySingleton* priorityClass);;

		virtual void OnStartUp();
		virtual void OnTerminate();
	protected:
		void ConstructGlobalObject();
		void DestroyGlobalObject();
		void CreateClassConstructionOrder(IPrioritySingleton* priorityClass);
		void CreateClassDestructionOrder(IPrioritySingleton* priorityClass);

		Detail::ManagedClassNode* m_pConstructionOrderHead{};
		Detail::ManagedClassNode* m_pConstructionOrderTail{};
		Detail::ManagedClassNode* m_pDestructionOrderHead{};
		Detail::ManagedClassNode* m_pDestructionOrderTail{};
		int m_iCode;	// 시스템을 구분하는 ID값
		bool m_bStarted{};
	};


	inline SystemAbstract* SystemMap_v[Detail::MaxSystemCode_v]{};

	template <const char* = nullptr>
	class System {};
	
} // namespace JCore


#endif