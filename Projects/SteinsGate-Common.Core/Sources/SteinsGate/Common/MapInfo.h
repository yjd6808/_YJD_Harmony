/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

//////////////////////////////////////////////////////////////////////////////////////////
struct MapObjectPositionInfo
{
	int code_;
	int x_;
	int y_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct MapAreaInfo
{
	MapAreaInfo(int _areaHeight)
	: area_(_areaHeight)
	{
	}

	SGVector<SGString> area_;

	bool CheckWall(float _x, float _y) const;

	int GetAreaHorizontolBlockCount()
	{
		return area_[0].Length();
	}

	int GetAreaVerticalBlockCount()
	{
		return area_.Size();
	}

	float GetAreaWidth();
	float GetAreaHeight();
};

struct MapInfo : ConfigDataAbstract
{
	MapInfo(int _npcCount, int _mapObjectCount)
	: npcList_(_npcCount)
	, mapObjectList_(_mapObjectCount)
	{
	}

	~MapInfo() override = default;

	SGString name_;
	MapType_t type_ = MapType::Private;
	int tileWidth_ = 0;
	int tileHeight_ = 0;
	int tileArray_[10][10]{};
	int physicsCode_ = 0;
	SGVector<MapObjectPositionInfo> npcList_;
	SGVector<MapObjectPositionInfo> mapObjectList_;
};
