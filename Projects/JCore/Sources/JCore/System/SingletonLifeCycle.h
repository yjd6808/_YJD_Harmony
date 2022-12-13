/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:49:58 PM
 * =====================
 * 생성자, 소멸자 호출 순서를 고정시키기 위함
 * 소멸자 호출 순서는 방법이 쉽게 떠올랐는데
 * 생성자 호출 순서를 만드는건 쉽지 않았다.
 *
 * class A : ClassLifeCycle<A, 1, 1>
 * class B : ClassLifeCycle<A, 6, 1>
 * class C : ClassLifeCycle<A, 3, 1>
 *
 * 프로그램 실행시 A => C => B 순으로 생성됨
 */


#pragma once


#include <JCore/System/JCoreSystem.h>

namespace JCore {

	struct ISingletonLifeCycle
	{
		virtual void Construct() = 0;
		virtual void Destroy() = 0;

		virtual int GetConstructionPriority() = 0;
		virtual int GetDestructionPriority() = 0;
		virtual ~ISingletonLifeCycle() = default;
	};

	template<typename T, Int32 ConstructionPriority, Int32 DestructionPriority>
	class SingletonLifeCycle : public ISingletonLifeCycle
	{
		using TClassType = SingletonLifeCycle<T, ConstructionPriority, DestructionPriority>;
	public:
		static TClassType& GetInstance() {
			DebugAssertMessage(ms_pDefaultValue != nullptr, "아직 초기화가 이뤄지지 않았습니다.");
			return reinterpret_cast<TClassType&>(ms_pDefaultValue);
		}

		int GetConstructionPriority() override { return ConstructionPriority; }
		int GetDestructionPriority() override { return DestructionPriority; }
	private:
		void Construct() override {
			DebugAssertMessage(ms_pDefaultValue == nullptr, "무조건 한번만 호출되어야합니다.");
			ms_pDefaultValue = new T();
		}

		void Destroy() override {
			DeleteSafe(ms_pDefaultValue);
		}
	private:
		struct ConstructorCall {
			ConstructorCall() {
				static TClassType _;
				g_JCoreSystem.RegisterSingletonLifeCycle(&_);
			}
		};

		inline static ISingletonLifeCycle* ms_pDefaultValue{};
		inline static ConstructorCall ms_ConstructorCaller{};
	};

	
} // namespace JCore
