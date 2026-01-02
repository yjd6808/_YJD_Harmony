/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:01:39 PM
 * =====================
 *
 */


#pragma once

#include <jc/Define.h>

JC_SENUM_BEGIN(MapObjectType)
	Obstacle,
	Begin = Obstacle,
	Gate,
	End = Gate,
JC_SENUM_END(MapObjectType)

// ==========================================================================================

JC_SENUM_BEGIN(MapType)
	Dungeon,
	Private,
	Town
JC_SENUM_END(MapType)

// ==========================================================================================

JC_SENUM_BEGIN(MapBundleType)
	Dungeon,
	Town
JC_SENUM_END(MapBundleType)

// ==========================================================================================
