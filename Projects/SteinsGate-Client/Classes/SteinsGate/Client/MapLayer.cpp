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


// ==============================================================================================================================
// 
// ==============================================================================================================================

//////////////////////////////////////////////////////////////////////////////////////////
MapLayer* MapLayer::create()
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
: m_pMapInfo(nullptr)
, m_pActorBox(nullptr)
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

	m_pActorBox = ActorManager::Get();
	m_pCamera = MimicCamera::create();
	this->addChild(m_pCamera);
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	HostPlayer::Get()->onKeyPressed(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	HostPlayer::Get()->onKeyReleased(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
MapPhysicsInfo* MapLayer::getMapPhysicsInfo() const
{
	return m_pMapPhysicsInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapAreaInfo* MapLayer::getMapAreaInfo() const
{
	return m_pMapAreaInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
MapInfo* MapLayer::getMapInfo() const
{
	return m_pMapInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
MimicCamera* MapLayer::getCamera() const
{
	return m_pCamera;
}


//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::update(float _dt)
{
	m_pActorBox->update(_dt);
	m_pCamera->update(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MapLayer::loadMap(int _mapCode)
{
	m_pMapInfo = Core::DataManager->getMapInfo(_mapCode);
	m_pMapAreaInfo = Core::DataManager->getMapAreaInfo(_mapCode);
	m_pMapPhysicsInfo = Core::DataManager->getMapPhysicsInfo(m_pMapInfo->physicsCode_);

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = m_pMapInfo->tileHeight_ - 1, k = 0; i >= 0; --i, ++k)
	{
		for (int j = 0; j < m_pMapInfo->tileWidth_; j++)
		{
			const float tileXPos = Const::Map::TileWidth * j;
			const float tileYPos = Const::Map::TileHeight * k;

			const TileInfo* pTileInfo = Core::DataManager->getTileInfo(m_pMapInfo->tileArray_[i][j]);
			FrameTexture* pFrameTexture = Core::Contents.PackManager->getPack(pTileInfo->SgaIndex)->createFrameTexture(
				pTileInfo->ImgIndex, pTileInfo->SpriteIndex);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->getTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(tileXPos, tileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < m_pMapInfo->mapObjectList_.Size(); ++i)
	{
		MapObjectPositionInfo& objectInfo = m_pMapInfo->mapObjectList_[i];
		m_pActorBox->createMapObjectOnMap(objectInfo.code_, objectInfo.x_, objectInfo.y_);
	}

	// NPC 로딩

	// 몬스터 로딩
	m_pActorBox->createMonsterOnMap(2, 1, 600, 350); // 테스트 몬스터 생성
	m_bMapLoaded = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
Character* MapLayer::findNearestCharacterInRadious(Actor* _pStdActor, float _radious, float& _enemyDist)
{
	int allyFlag = _pStdActor->getAllyFlag();
	SGVec2 stdPos = _pStdActor->getPositionRealCenter();
	Character* pNearestCharacter = nullptr;
	float minDist = FLT_MAX;
	CharacterList& characterList = m_pActorBox->getCharacterList();

	for (int i = 0; i < characterList.Size(); ++i)
	{
		auto pCharacter = characterList[i];

		if (pCharacter == _pStdActor)
			continue;

		float dist = stdPos.distance(pCharacter->getPositionRealCenter());

		if (dist < _radious && pCharacter->getAllyFlag() != allyFlag && dist < minDist)
		{
			minDist = dist;
			pNearestCharacter = pCharacter;
		}
	}

	_enemyDist = minDist;
	return pNearestCharacter;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::collectEnemiesInActorRect(
	Actor* _pAttacker,
	const ActorRect& _absoluteActorRect,
	JCORE_OUT SGVector<HitInfo>& _hitTargets)
{
	ActorList& physcisActorList = m_pActorBox->getPhysicsActorList();
	bool find = false;

	for (int i = 0; i < physcisActorList.Size(); ++i)
	{
		auto pHitTarget = physcisActorList[i]; // 공격받을 대상
		SGRect hitRect;
		SpriteDirection_t hitDirection;

		if (pHitTarget->getAllyFlag() == _pAttacker->getAllyFlag())
			continue;

		// 몬스터 기준으로 플레이어 충돌이라
		// eHitDirection은 플레이어의 충돌방향이 되므로, 반대로 돌려줘야함.
		if (pHitTarget->isCollide(_absoluteActorRect, hitDirection, hitRect))
		{
			_hitTargets.PushBack({ _pAttacker, pHitTarget, SpriteDirection::Reverse[hitDirection], hitRect, nullptr });
			find = true;
		}
	}

	return find;
}


//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::collectEnemiesInActor(Actor* _pCollector, JCORE_OUT SGVector<HitInfo>& _hitTargets)
{
	return collectEnemiesInActorRect(_pCollector, _pCollector->getActorRect(), _hitTargets);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MapLayer::isCollideWithMapObjects(const SGRect& _rect) const
{
	MapObjectList& collidableMapObjects = m_pActorBox->getCollidableMapObjectList();

	for (int i = 0; i < collidableMapObjects.Size(); ++i)
	{
		SGRect thicknessBox = collidableMapObjects[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(_rect))
			return true;
	}

	return false;
}
