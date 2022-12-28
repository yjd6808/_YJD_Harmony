/*
 * 작성자: 윤정도
 * 생성일: 12/16/2022 8:58:31 PM
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
#include <JCore/System/System.h>
#include <JCore/System/PrioritySingleton.h>

#if 0
namespace JCore {
	namespace Detail {
		void RegisterPrioritySingleton(const int systemCode, IPrioritySingleton* managedSingleton) {
			DebugAssertMessage(systemCode >= 0 && systemCode <= Detail::MaxSystemCode_v, "시스템 코드번호는 0 ~ 64사이의 값입니다.");
			DebugAssertMessage(SystemMap_v[systemCode] != nullptr, "해당 코드번호에 해당하는 시스템이 존재하지 않습니다.");
			SystemMap_v[systemCode]->RegisterPrioritySingleton(managedSingleton);
		}
	}
}
#endif