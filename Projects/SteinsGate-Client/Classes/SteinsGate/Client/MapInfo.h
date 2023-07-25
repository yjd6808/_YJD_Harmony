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

struct MapInfo : ConfigDataAbstract
{
	MapInfo(int areaHeight, int npcCount, int mapObjectCount)
		: Area(areaHeight)
		, NpcList(npcCount)
		, MapObjectList(mapObjectCount) {}
	~MapInfo() override = default;

	SGString Name;
	int MapWidth;
	int MapHeight;
	int TileWidth;
	int TileHeight;
	int TileArray[10][10];
	float Gravity;
	float Friction;
	float ElasticityDividedForce;
	SGVector<SGString> Area;
	SGVector<MapObjectPositionInfo> NpcList;
	SGVector<MapObjectPositionInfo> MapObjectList;

	bool checkWall(float x, float y);
	bool checkWall(const SGVec2& pos);
	int getAreaWidth() { return Area[0].Length(); }
	int getAreaHeight() { return Area.Size(); }
	SGSize getMapSize() { return { float(MapWidth), float(MapHeight) }; }
};



