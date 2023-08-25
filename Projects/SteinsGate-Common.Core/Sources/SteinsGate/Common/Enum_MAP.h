/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:01:39 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(MapObjectType)
Obstacle,
Begin = Obstacle,
Gate,
End = Gate,
JCORE_SENUM_END(MapObjectType)

// ==========================================================================================

JCORE_SENUM_BEGIN(MapType)
Dungeon,
Private,
Town
JCORE_SENUM_END(MapType)

// ==========================================================================================

JCORE_SENUM_BEGIN(MapBundleType)
Dungeon,
Town
JCORE_SENUM_END(MapBundleType)

// ==========================================================================================