/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:37:58 AM
 * =====================
 */


#pragma once

#include <sg/_Struct/SteinsGate_CharBase.h>

struct ThicknessBox
{
	float relativeX_;
	float relativeY_;
	float width_;
	float height_;

	ThicknessBox()
	: relativeX_(0)
	, relativeY_(0)
	, width_(0)
	, height_(0)
	{}

	ThicknessBox(float _relativeX, float _relativeY, float _width, float _height)
	: relativeX_(_relativeX)
	, relativeY_(_relativeY)
	, width_(_width)
	, height_(_height)
	{}
};

struct ActorPartSpriteData
{
	ActorPartSpriteData();
	ActorPartSpriteData(int _zOrder, int _sga, int _img);

	// 스프라이트 정보(필수)
	int sgaIndex_;
	int imgIndex_;

	// 미입력시 순서대로 1씩증가
	int zOrder_;
	SgaSpriteRect customSizeInfo_;
};

struct ActorSpriteData
{
	ActorSpriteData(ActorPartSpritePositioningRule_t _positioningRule, int _partCount, int _animationCount)
		: positioningRule_(_positioningRule)
		, parts_(_partCount)
		, animations_(_animationCount)
	{
	}

	ActorPartSpritePositioningRule_t positioningRule_;
	jc::Vector<ActorPartSpriteData> parts_;
	jc::Vector<AnimationInfo> animations_;
};
