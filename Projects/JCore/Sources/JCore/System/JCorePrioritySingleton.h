/*
 * 작성자: 윤정도
 * 생성일: 12/17/2022 1:50:52 AM
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


#pragma once

#if 0

#include <JCore/System/PrioritySingleton.h>
#include <JCore/System/JCoreSystem.h>


namespace JCore {
	template <Int32U SystemCode, typename T, Int32 ConstructionPriority, Int32 DestructionPriority>
	using JCorePSingleton = PrioritySingleton<SystemCode_v, T, ConstructionPriority, DestructionPriority>;

	template <Int32U SystemCode, typename T, Int32 DestructionPriority>
	using JCoreDPSingleton = PrioritySingleton<SystemCode_v, T, Detail::IgnorePriority, DestructionPriority>;

	template <Int32U SystemCode, typename T, Int32 ConstructionPriority>
	using JCoreCPSingleton = PrioritySingleton<SystemCode_v, T, ConstructionPriority, Detail::IgnorePriority>;

	template <Int32U SystemCode, typename T>
	using JCoreSingleton = PrioritySingleton<SystemCode_v, T, Detail::IgnorePriority, Detail::IgnorePriority>;
} // namespace JCore

#endif
