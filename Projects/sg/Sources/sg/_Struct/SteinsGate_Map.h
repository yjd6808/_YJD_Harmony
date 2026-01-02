/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

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

	jc::Vector<jc::String> area_;

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

struct MapInfo : SDescBase
{
	MapInfo(int _npcCount, int _mapObjectCount)
	: npcList_(_npcCount)
	, mapObjectList_(_mapObjectCount)
	{
	}

	~MapInfo() override = default;

	jc::String name_;
	MapType_t type_ = MapType::Private;
	int tileWidth_ = 0;
	int tileHeight_ = 0;
	int tileArray_[10][10]{};
	int physicsCode_ = 0;
	jc::Vector<MapObjectPositionInfo> npcList_;
	jc::Vector<MapObjectPositionInfo> mapObjectList_;
};
