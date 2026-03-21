/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:24:01 AM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "MoveComponent.h"

#include "sgcl/_Layer/Layer_Map.h"

USING_NS_JC;
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////////////////////
MoveComponent::MoveComponent(Actor* _pActor)
: ActorComponent(_pActor)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::Initialize()
{
	speed_.x = 0;
	speed_.y = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::OnUpdate(float _dt)
{
	MapLayer* pMapLayer = pActor_->GetMapLayer();
	if (pMapLayer == nullptr)
	{
		return;
	}

	MapAreaInfo* pAreaInfo = pMapLayer->GetMapAreaInfo();
	if (pAreaInfo == nullptr)
	{
		return;
	}

	// 좌,우 | 위,아래 독립적으로 계산해야함
	// 한개로만 하게되면 예를들어 Left, Down을 동시에 눌렀을 때
	// thickness.origin.x -= fSpeedX 적용된 값이 Down 체크시에도 적용되어버려서
	// 이 적용된 값 때문에 Down에서 lb, rb 충돌 체크가 항상 참이 되어버림
	// --------------------------------------------------------------
	//  23/01/28 -> 좌,우,위,아래 모두 독립적으로 가능하도록 추가
	cc::rect thicknessPosLR = pActor_->GetThicknessBoxRect();
	cc::rect thicknessPosUD = thicknessPosLR;

	thicknessPosLR.origin.x += speed_.x;
	thicknessPosUD.origin.y += speed_.y;

	if (speed_.x < 0)
	{
		UpdateLeftMove(pMapLayer, pAreaInfo, thicknessPosLR);
	}
	else
	{
		UpdateRightMove(pMapLayer, pAreaInfo, thicknessPosLR);
	}

	if (speed_.y < 0)
	{
		UpdateDownMove(pMapLayer, pAreaInfo, thicknessPosUD);
	}
	else
	{
		UpdateUpMove(pMapLayer, pAreaInfo, thicknessPosUD);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::UpdateLeftMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const cc::rect& _thicknessRect)
{
	cc::vec2 lb{ _thicknessRect.origin.x, _thicknessRect.origin.y };
	cc::vec2 lt{ _thicknessRect.origin.x, _thicknessRect.origin.y + _thicknessRect.size.height };

	// lb, lt 체크
	if (_pAreaInfo->CheckWall(lb.x, lb.y) || _pAreaInfo->CheckWall(lt.x, lt.y) || _pMapLayer->
		IsCollideWithMapObjects(_thicknessRect))
	{
		return;
	}

	pActor_->SetPositionRealX(_thicknessRect.origin.x);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::UpdateRightMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const cc::rect& _thicknessRect)
{
	cc::vec2 rb{ _thicknessRect.origin.x + _thicknessRect.size.width, _thicknessRect.origin.y };
	cc::vec2 rt{
		_thicknessRect.origin.x + _thicknessRect.size.width, _thicknessRect.origin.y + _thicknessRect.size.height
	};

	// rb, rt 체크
	if (_pAreaInfo->CheckWall(rb.x, rb.y) || _pAreaInfo->CheckWall(rt.x, rt.y) || _pMapLayer->
		IsCollideWithMapObjects(_thicknessRect))
	{
		return;
	}

	pActor_->SetPositionRealX(_thicknessRect.origin.x);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::UpdateUpMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const cc::rect& _thicknessRect)
{
	cc::vec2 lt{ _thicknessRect.origin.x, _thicknessRect.origin.y + _thicknessRect.size.height };
	cc::vec2 rt{
		_thicknessRect.origin.x + _thicknessRect.size.width, _thicknessRect.origin.y + _thicknessRect.size.height
	};

	// lt, rt 체크
	if (_pAreaInfo->CheckWall(lt.x, lt.y) || _pAreaInfo->CheckWall(rt.x, rt.y) || _pMapLayer->
		IsCollideWithMapObjects(_thicknessRect))
	{
		return;
	}

	pActor_->SetPositionRealY(_thicknessRect.origin.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::UpdateDownMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const cc::rect& _thicknessRect)
{
	cc::vec2 lb{ _thicknessRect.origin.x, _thicknessRect.origin.y };
	cc::vec2 rb{ _thicknessRect.origin.x + _thicknessRect.size.width, _thicknessRect.origin.y };

	// lb, rb 체크
	if (_pAreaInfo->CheckWall(lb.x, lb.y) || _pAreaInfo->CheckWall(rb.x, rb.y) || _pMapLayer->
		IsCollideWithMapObjects(_thicknessRect))
	{
		return;
	}

	pActor_->SetPositionRealY(_thicknessRect.origin.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::SetSpeed(const cc::vec2& _speed)
{
	speed_ = _speed;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MoveComponent::SetSpeed(float _x, float _y)
{
	speed_.x = _x;
	speed_.y = _y;
}

//////////////////////////////////////////////////////////////////////////////////////////
const cc::vec2& MoveComponent::GetSpeed() const
{
	return speed_;
}
