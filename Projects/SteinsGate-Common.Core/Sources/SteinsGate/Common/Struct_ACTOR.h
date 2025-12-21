/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:55:01 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Const.h>
#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Common/SgaSpriteRect.h>
#include <SteinsGate/Common/Struct_ANIMATION.h>

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
	{
	}

	ThicknessBox(float _relativeX, float _relativeY, float _width, float _height)
	: relativeX_(_relativeX)
	, relativeY_(_relativeY)
	, width_(_width)
	, height_(_height)
	{
	}
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
	SGVector<ActorPartSpriteData> parts_;
	SGVector<AnimationInfo> animations_;
};

using ActorSpriteDataPtr = JCore::SharedPtr<ActorSpriteData>;

// 캐릭터의 비주얼 아이템(아바타 혹은 무기)는 여러 조합을 만들기위해 하나의 부위가 여러개의 쉐이프(그냥 대충 지은 이름)로 구성된다.
// 그래서 모든 아바타+무기는 각각 최대 3개씩의 파츠 데이터를 가질 수 있도록 구성하였다.
// 예를들어 거너 아바타 하의는 2개의 파츠 데이터로 구성되었다고하자.
// shape: 2203, shape_alpha: ab라고 하면
// pants_2203a, pants_2203b 이미지팩 각각의 ActorPartSpriteData 2개를 뭉뚱그려서 VisualData라 명명하였다.
using VisualData = ActorPartSpriteData[Const::Visual::MaxShapeCount];
using VisualInfo = JCore::Vector<ActorPartSpriteData>;
