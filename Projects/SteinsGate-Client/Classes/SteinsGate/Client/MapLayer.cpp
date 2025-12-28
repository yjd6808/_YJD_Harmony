/*
 * 작성자 : 윤정도
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "MapLayer.h"

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/RectPoly.h>

#include <SteinsGate/Client/Define_Action.h>


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
MapLayer* MapLayer::Create()
{
	MapLayer* pMap = dbg_new MapLayer;

	if (pMap && pMap->init())
	{
		pMap->autorelease();
		return pMap;
	}

	JCORE_DELETE_SAFE(pMap);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapLayer::MapLayer()
: pMapInfo_(nullptr)
, pActorBox_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MapLayer::~MapLayer()
{
	_LogDebug_("맵 레이어 소멸");
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	pActorBox_ = ActorManager::Get();
	pCamera_ = MimicCamera::Create();
	this->addChild(pCamera_);
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	HostPlayer::Get()->OnKeyPressed(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	HostPlayer::Get()->OnKeyReleased(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
MapPhysicsInfo* MapLayer::GetMapPhysicsInfo() const
{
	return pMapPhysicsInfo_;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* MapLayer::GetMapAreaInfo() const
{
	return pMapAreaInfo_;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* MapLayer::GetMapInfo() const
{
	return pMapInfo_;
}

//////////////////////////////////////////////////////////////////////////////////////////
MimicCamera* MapLayer::GetCamera() const
{
	return pCamera_;
}


//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::update(float _dt)
{
	pActorBox_->Update(_dt);
	pCamera_->update(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::LoadMap(int _mapCode)
{
	pMapInfo_ = Core::DataManager->GetMapInfo(_mapCode);
	pMapAreaInfo_ = Core::DataManager->GetMapAreaInfo(_mapCode);
	pMapPhysicsInfo_ = Core::DataManager->GetMapPhysicsInfo(pMapInfo_->physicsCode_);

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = pMapInfo_->tileHeight_ - 1, k = 0; i >= 0; --i, ++k)
	{
		for (int j = 0; j < pMapInfo_->tileWidth_; j++)
		{
			const float tileXPos = Const::Map::TileWidth * j;
			const float tileYPos = Const::Map::TileHeight * k;

			const TileInfo* pTileInfo = Core::DataManager->GetTileInfo(pMapInfo_->tileArray_[i][j]);
			FrameTexture* pFrameTexture = Core::Contents.PackManager->GetPack(pTileInfo->sgaIndex_)->CreateFrameTexture(
				pTileInfo->imgIndex_, pTileInfo->spriteIndex_);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->GetTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(tileXPos, tileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < pMapInfo_->mapObjectList_.Size(); ++i)
	{
		MapObjectPositionInfo& objectInfo = pMapInfo_->mapObjectList_[i];
		pActorBox_->CreateMapObjectOnMap(objectInfo.code_, objectInfo.x_, objectInfo.y_);
	}

	// NPC 로딩

	// 몬스터 로딩
	pActorBox_->CreateMonsterOnMap(2, 1, 600, 350); // 테스트 몬스터 생성
	isMapLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
Character* MapLayer::FindNearestCharacterInRadious(Actor* _pStdActor, float _radious, float& _enemyDist)
{
	int allyFlag = _pStdActor->GetAllyFlag();
	SGVec2 stdPos = _pStdActor->GetPositionRealCenter();
	Character* pNearestCharacter = nullptr;
	float minDist = FLT_MAX;
	CharacterList& characterList = pActorBox_->GetCharacterList();

	for (int i = 0; i < characterList.Size(); ++i)
	{
		auto pCharacter = characterList[i];

		if (pCharacter == _pStdActor)
			continue;

		float dist = stdPos.distance(pCharacter->GetPositionRealCenter());

		if (dist < _radious && pCharacter->GetAllyFlag() != allyFlag && dist < minDist)
		{
			minDist = dist;
			pNearestCharacter = pCharacter;
		}
	}

	_enemyDist = minDist;
	return pNearestCharacter;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::CollectEnemiesInActorRect(
	Actor* _pAttacker,
	const ActorRect& _absoluteActorRect,
	OUT SGVector<HitInfo>& _hitTargets)
{
	ActorList& physcisActorList = pActorBox_->GetPhysicsActorList();
	bool find = false;

	for (int i = 0; i < physcisActorList.Size(); ++i)
	{
		auto pHitTarget = physcisActorList[i]; // 공격받을 대상
		SGRect hitRect;
		SpriteDirection_t hitDirection;

		if (pHitTarget->GetAllyFlag() == _pAttacker->GetAllyFlag())
			continue;

		// 몬스터 기준으로 플레이어 충돌이라
		// eHitDirection은 플레이어의 충돌방향이 되므로, 반대로 돌려줘야함.
		if (pHitTarget->IsCollide(_absoluteActorRect, hitDirection, hitRect))
		{
			_hitTargets.PushBack({ _pAttacker, pHitTarget, SpriteDirection::Reverse[hitDirection], hitRect, nullptr });
			find = true;
		}
	}

	return find;
}


//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::CollectEnemiesInActor(Actor* _pCollector, OUT SGVector<HitInfo>& _hitTargets)
{
	return CollectEnemiesInActorRect(_pCollector, _pCollector->GetActorRect(), _hitTargets);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::IsCollideWithMapObjects(const SGRect& _rect) const
{
	MapObjectList& collidableMapObjects = pActorBox_->GetCollidableMapObjectList();

	for (int i = 0; i < collidableMapObjects.Size(); ++i)
	{
		SGRect thicknessBox = collidableMapObjects[i]->GetThicknessBoxRect();

		if (thicknessBox.intersectsRect(_rect))
			return true;
	}

	return false;
}
