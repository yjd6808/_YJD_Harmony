/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 6:32:00 AM
 * =====================
 *
 */

#include "Core.h"
#include "MimicCamera.h"
#include "GameCoreHeader.h"

#include <sgcl/MapLayer.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MimicCamera::MimicCamera()
: pTarget_(nullptr)
, pMapLayer_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MimicCamera* MimicCamera::Create()
{
	MimicCamera* pInst = dbg_new MimicCamera;
	pInst->autorelease();
	return pInst;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MimicCamera::SetFollowTarget(Actor* _pTarget)
{
	pTarget_ = _pTarget;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MimicCamera::update(float _dt)
{
	UpdateView(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TODO: 무빙 자연스럽게
void MimicCamera::UpdateView(float _dt)
{
	if (pTarget_ == nullptr)
		return;

	MapLayer* pMapLayer = dynamic_cast<MapLayer*>(_parent);

	if (pMapLayer == nullptr)
	{
		jc_assert(false);
		return;
	}

	MapAreaInfo* pAreaInfo = pMapLayer->GetMapAreaInfo();

	if (pAreaInfo == nullptr)
		return;

	cc::size mapSize{ pAreaInfo->GetAreaWidth(), pAreaInfo->GetAreaHeight() };
	cc::size winSize = _director->getWinSize();

	Vec2 mapLayerPos = pMapLayer->getPosition();
	Vec2 playerNodePos = pTarget_->GetPositionRealCenter();
	Vec2 playerNodePosOnScene = playerNodePos + mapLayerPos;

	Size nineRectSize = winSize / 3.0f;
	Rect nineRect = { nineRectSize, nineRectSize };

	// 나인렉트 위쪽 경계를 벗어난 경우
	if (playerNodePosOnScene.y > nineRect.getMaxY())
	{
		mapLayerPos.y -= playerNodePosOnScene.y - nineRect.getMaxY();
	}
	// 나인렉트 아래쪽 경계를 벗어난 경우
	else if (playerNodePosOnScene.y < nineRect.getMinY())
	{
		mapLayerPos.y += nineRect.getMinY() - playerNodePosOnScene.y;
	}

	// 나인렉트 오른쪽 경계를 벗어난 경우
	if (playerNodePosOnScene.x > nineRect.getMaxX())
	{
		mapLayerPos.x -= playerNodePosOnScene.x - nineRect.getMaxX();
	}
	// 나인렉트 왼쪽 경계를 벗어난 경우
	else if (playerNodePosOnScene.x < nineRect.getMinX())
	{
		mapLayerPos.x += nineRect.getMinX() - playerNodePosOnScene.x;
	}

	// 위쪽 한계, 오른쪽 한계를 미리 계산해놓자.
	// 내가 우측으로 500만큼 가면 mapLayerPos.x는 -500임 반대라는걸 유념할 것
	float rightX = -(mapSize.width - winSize.width);
	float topY = -(mapSize.height - winSize.height);

	if (mapLayerPos.x > 0.0f)
	{
		mapLayerPos.x = 0.0f;
	}
	else if (mapLayerPos.x < rightX)
	{
		mapLayerPos.x = rightX;

		// 맵 높이가 화면 높이보다 더 작은 경우
		if (rightX > 0)
		{
			mapLayerPos.x = 0.0f;
		}
	}

	// 아래
	if (mapLayerPos.y > 0.0f)
	{
		mapLayerPos.y = 0.0f;
	}
	// 위
	else if (mapLayerPos.y < topY)
	{
		mapLayerPos.y = topY;

		// 맵 높이가 화면 높이보다 더 작은 경우
		if (topY > 0)
		{
			mapLayerPos.y = 0.0f;
		}
	}

	pMapLayer->setPosition(mapLayerPos);
}
