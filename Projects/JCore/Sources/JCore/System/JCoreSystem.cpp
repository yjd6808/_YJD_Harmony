/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 9:08:07 PM
 * =====================
 */

#include <JCore/Core.h>
#include <JCore/System/SingletonLifeCycle.h>
#include <JCore/System/JCoreSystem.h>


namespace JCore {
	void System<SystemName>::Initialize() {
		g_JCoreSystem.OnStartUp();
	}

	// ========================================================================
	// Private 기능
	// ========================================================================

	void System<SystemName>::RegisterSingletonLifeCycle(ISingletonLifeCycle* cycledClass) {
		g_JCoreSystem.CreateClassConstructionOrder(cycledClass);
		g_JCoreSystem.CreateClassDestructionOrder(cycledClass);
	}

	void System<SystemName>::OnStartUp() {
		g_JCoreSystem.ConstructGlobalObject();
	}

	void System<SystemName>::OnTerminate() {
		g_JCoreSystem.DestroyGlobalObject();
	}

	
}

