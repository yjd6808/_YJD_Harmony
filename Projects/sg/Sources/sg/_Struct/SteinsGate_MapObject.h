/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 11:32:50 AM
 * =====================
 *
 */


#pragma once


#include <sg/_Util/DescBase.h>
#include <sgcl/Struct.h>

//////////////////////////////////////////////////////////////////////////////////////////
struct MapObjectInfo : SDescBase
{
	MapObjectInfo()
	: isHittable_(false)
	, isColliadalble_(false)
	, isZOrederable_(false)
	, type_(MapObjectType::Gate)
	, pSpriteData_(nullptr)
	{
	}

	~MapObjectInfo() override { JC_DELETE_SAFE(pSpriteData_); }

	bool isHittable_;
	bool isColliadalble_;
	bool isZOrederable_;

	jc::String name_;
	MapObjectType_t type_;
	ThicknessBox thicknessBox_;
	ActorSpriteData* pSpriteData_;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct MapObjectObstacleInfo : MapObjectInfo
{
	~MapObjectObstacleInfo() override = default;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct MapObjectGateInfo : MapObjectInfo
{
	~MapObjectGateInfo() override = default;
};
