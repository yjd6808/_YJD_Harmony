/*
 * 작성자: 윤정도
 * 생성일: 12/17/2022 1:50:52 AM
 * =====================
 *
 */


#pragma once

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


