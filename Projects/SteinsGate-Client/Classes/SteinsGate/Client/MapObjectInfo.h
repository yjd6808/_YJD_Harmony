/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:50 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Client/Struct.h>

struct MapObjectInfo : ConfigDataAbstract
{
	MapObjectInfo()
		: Hitable(false)
		, Colliadalble(false)
		, ZOrederable(false)
		, SpriteData(nullptr) {}

	~MapObjectInfo() override { JCORE_DELETE_SAFE(SpriteData); }

	bool Hitable;
	bool Colliadalble;
	bool ZOrederable;

	SGString Name;
	MapObjectType_t Type;
	ThicknessBox ThicknessBox;
	ActorSpriteData* SpriteData;
};

struct MapObjectObstacleInfo : MapObjectInfo
{
	~MapObjectObstacleInfo() override = default;
};

struct MapObjectGateInfo : MapObjectInfo
{
	~MapObjectGateInfo() override = default;
};