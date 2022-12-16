/*
 * 작성자: 윤정도
 * 생성일: 12/16/2022 8:58:31 PM
 * =====================
 *
 */
#include <JCore/Core.h>
#include <JCore/System/System.h>
#include <JCore/System/PrioritySingleton.h>

namespace JCore {
	namespace Detail {
		void RegisterPrioritySingleton(const int systemCode, IPrioritySingleton* managedSingleton) {
			DebugAssertMessage(systemCode >= 0 && systemCode <= Detail::MaxSystemCode_v, "시스템 코드번호는 0 ~ 64사이의 값입니다.");
			DebugAssertMessage(SystemMap_v[systemCode] != nullptr, "해당 코드번호에 해당하는 시스템이 존재하지 않습니다.");
			SystemMap_v[systemCode]->RegisterPrioritySingleton(managedSingleton);
		}
	}
}