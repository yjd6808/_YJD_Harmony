/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:49:58 PM
 * ===============================================================================
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
 * =======================================================================================
 * 내가 사용할 통합 싱글톤 기능 구현
 * 따로 우선순위 싱글톤 자료를 참고하지 않고 내 생각대로만 구현한 것이므로 이것도
 * 나중에 실제 구현 참고해봐야할 듯?
 * 근데 우선순위 싱글톤이란게 있나?
 *
 *  - Singleton<T> 상속시에는 단순한 더블체크드롹킹 싱글톤
 *  - ConstructionPrioritySingleton<T, Int>를 상속시에는 생성자를 지정한 순서대로 호출해줌 (사용시 주의사항 읽을 것)
 *	- DestrunctionPrioritySingleton<T, Int>를 상속시에는 소멸자를 지정한 순서대로 호출해줌
 *	- PrioritySingleton<T, Int, Int>는 생성자, 소멸자 모두 순서를 정하고 싶을 때 사용
 *
 * ================================================================
 *
 * class A : PrioritySingleton<A, 1, 5>
 * class B : PrioritySingleton<B, 6, 3>
 * class C : PrioritySingleton<C, 3, 1>
 *
 * 프로그램 실행시 A => C => B 순으로 생성됨
 * 프로그램 실행시 C => B => A 순으로 소멸됨
 *
 * 숫자가 낮을수록 우선순위가 높음 (같을 경우 랜덤, 컴파일 순서룰 머릿속에 넣어넣고 있지 않으면 알 수 없음)
 * ================================================================
 * [생성자 호출 순서 싱글톤 사용시 주의사항]
 *  - 실질적으로 main() 함수에서 System::Initialize() 호출시 등록된 순서대로 생성을 진행해주기 때문에
 *	  이 기능을 사용하지 않는 전역 변수들보다 무조건 늦게 생성된다. 
 *	  따라서 이기능을 사용하게 된다면 프로그램 내에서 사용하는 전역 변수들은 이 기능을 상속받아서 구현하는게 안전하다.
 *	  그리고 다른 전역변수의 생성자에서 이 생성자 호출순서가 적ㅇ
 *  - 우선순위가 2인 생성자에서 우선순위가 1인 대상을 참조하는 것은 불가능하다. (생성자에서만)
 * ================================================================
 * [의문점]
 *  - 생성자 호출순서를 고정시키면 무슨 이점이 있을까?
 *	  필요한 적재적소에 그냥 생성되는게 베스트 아닐까?
 *
 *  - System::Initialize 호출없이 생성자 순서를 지정할 수 없을까?
 *    System<> 전역변수가 제일 마지막에 생성되도록 하면 되는데
 *	  그럴려면 제일 늦게 메모리에 올라가야하는데.. 어떻게 그걸 제어하지;
 *	  
 */


#pragma once

#if 0
#include <JCore/Sync/UnusedLock.h>
#include <JCore/Sync/NormalLock.h>

namespace JCore {
	struct IPrioritySingleton;
	namespace Detail {
		constexpr int IgnorePriority = -1;	// 생성: 생성자 우선순위 리스트에 등록안함. GetInstance() 호출시 즉시 생성
											// 삭제: 우선순위가 -1로 소멸자 리스트에 등록 따라서 최상위 우선순위로 삭제.
											// IgnorePriority 들끼리는 뭐가 먼저 삭제될지 모름
		void RegisterPrioritySingleton(int SystemCode, IPrioritySingleton* managedSingleton);
	}

	struct IPrioritySingleton
	{
		virtual void Construct() = 0;
		virtual void Destroy() = 0;
		virtual int GetConstructionPriority() = 0;
		virtual int GetDestructionPriority() = 0;
		virtual ~IPrioritySingleton() = default;
	};


	template<Int32U SystemCode, typename T, Int32 ConstructionPriority, Int32 DestructionPriority>
	class PrioritySingleton : public IPrioritySingleton
	{
		static constexpr bool IsEnabledConstruction = ConstructionPriority >= 0;		// 생성자 호출을 시스템에 위임한 경우
		static constexpr bool IsEnabledDestruction = DestructionPriority >= 0;			// 소멸자 호출을 시스템에 위임한 경우

		using TLock = JCore::Conditional_t<IsEnabledConstruction, JCore::UnusedLock, JCore::NormalLock>;
		using Type = PrioritySingleton<SystemCode, T, ConstructionPriority, DestructionPriority>;
	public:
		static T& GetInstance() {

			if constexpr (!IsEnabledConstruction) {
				if (ms_pDefaultValue == nullptr) {
					JCore::LockGuard<TLock> guard(ms_Lock);
					if (ms_pDefaultValue == nullptr) {
						ms_pDefaultValue = new T();
					}
				}
			}

			DebugAssertMessage(ms_pDefaultValue != nullptr, "아직 초기화가 이뤄지지 않았습니다. System<>::Initialize() 이전에 호출되었는지 확인해주세요.");
			return *reinterpret_cast<T*>(ms_pDefaultValue);
		}

		int GetConstructionPriority() override { return ConstructionPriority; }
		int GetDestructionPriority() override { return DestructionPriority; }
	protected:
		PrioritySingleton(){}

		void Construct() override {
			if constexpr (ConstructionPriority > Detail::IgnorePriority) {
				DebugAssertMessage(ms_pDefaultValue == nullptr, "무조건 한번만 호출되어야합니다.");
				ms_pDefaultValue = new T();
			}
		}

		void Destroy() override {
			DeleteSafe(ms_pDefaultValue);
		}
	private:

		struct ConstructorCall {																
			ConstructorCall() {																	
				if constexpr (IsEnabledConstruction) {											
					static Type s_TypeHolder;													

					// System.h 의존성을 없애고
					// 여러 시스템에서 범용적으로 사용가능한 싱글톤 기능을 수행토록 하기 위해
					// 어쩔수없이 소스파일로 기능을 뺏다.
					// 우선순위 싱글톤 구현방법이 이 방식밖에 안떠올라서 마음에 안들지만 어쩔수가 없다.
					Detail::RegisterPrioritySingleton(SystemCode, &s_TypeHolder); 
				}
			}
		};

		inline static IPrioritySingleton* ms_pDefaultValue;
		inline static ConstructorCall ms_ConstructorCaller{};
		inline static TLock ms_Lock;
	};

	template <Int32U SystemCode, typename T, Int32 ConstructionPriority, Int32 DestructionPriority>
	using PSingleton = PrioritySingleton<SystemCode, T, ConstructionPriority, DestructionPriority>;

	template <Int32U SystemCode, typename T, Int32 DestructionPriority>
	using DPSingleton = PrioritySingleton<SystemCode, T, Detail::IgnorePriority, DestructionPriority>;

	template <Int32U SystemCode, typename T, Int32 ConstructionPriority>
	using CPSingleton = PrioritySingleton<SystemCode, T, ConstructionPriority, Detail::IgnorePriority>;

	template <Int32U SystemCode, typename T>
	using Singleton = PrioritySingleton<SystemCode, T, Detail::IgnorePriority, Detail::IgnorePriority>;

	
} // namespace JCore

#endif