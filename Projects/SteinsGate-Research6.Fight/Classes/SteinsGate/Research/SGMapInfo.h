/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>

struct SGMapObjectInfo
{
	int Code;
	int X;
	int Y;
};

struct SGMapInfo
{
	SGMapInfo(int areaHeight, int npcCount, int obstacleCount)
		: Area(areaHeight)
		, NpcList(npcCount)
		, ObstacleList(obstacleCount) {}

	int Code;
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
	SGVector<SGMapObjectInfo> NpcList;
	SGVector<SGMapObjectInfo> ObstacleList;

	bool checkWall(float x, float y);
	bool checkWall(const SGVec2& pos);
	int getAreaWidth() { return Area[0].Length(); }
	int getAreaHeight() { return Area.Size(); }
};



