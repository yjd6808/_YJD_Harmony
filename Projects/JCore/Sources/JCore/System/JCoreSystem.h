/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 6:26:07 PM
 * =====================
 * 
 */


#pragma once


#include <JCore/System/System.h>

namespace JCore {
	inline const char SystemName[] = "JCore";

	template <>
	class System<SystemName> : SystemAbstract
	{
	public:
		System() = default;
		~System() { OnTerminate(); }

		static void Initialize();
	private:
		void RegisterSingletonLifeCycle(ISingletonLifeCycle*) override;
		void OnStartUp();
		void OnTerminate();

		template <typename, Int, Int> friend class SingletonLifeCycle;
	};

	inline System<SystemName> g_JCoreSystem;
	using JCoreSystem = System<SystemName>;

} // namespace JCore

