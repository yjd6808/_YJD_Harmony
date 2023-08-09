/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct MapObjectPositionInfo
{
	int Code;
	int X;
	int Y;
};

struct MapAreaInfo
{
	MapAreaInfo(int areaHeight) : Area(areaHeight) {}
	SGVector<SGString> Area;

	bool checkWall(float x, float y) const;

	int getAreaHorizontolBlockCount() { return Area[0].Length(); }
	int getAreaVerticalBlockCount() { return Area.Size(); }
	float getAreaWidth();
	float getAreaHeight();
};

struct MapInfo : ConfigDataAbstract
{
	MapInfo(int npcCount, int mapObjectCount)
		: NpcList(npcCount)
		, MapObjectList(mapObjectCount) {}
	~MapInfo() override = default;

	SGString Name;
	MapType_t Type;
	int TileWidth;
	int TileHeight;
	int TileArray[10][10];
	int PhysicsCode;
	SGVector<MapObjectPositionInfo> NpcList;
	SGVector<MapObjectPositionInfo> MapObjectList;
};



