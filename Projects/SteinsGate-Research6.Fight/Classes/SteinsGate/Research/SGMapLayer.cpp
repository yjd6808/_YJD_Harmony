/*
 * 작성자 : 윤정도
 */

#include "Tutturu.h"
#include "SGMapLayer.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGActionDefine.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>
#include <SteinsGate/Common/Engine/RectPoly.h>

#include "GameScene.h"


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGMapLayer* SGMapLayer::create(int mapCode) {
	SGMapLayer* pMap = new (std::nothrow) SGMapLayer;

	if (pMap && pMap->init()) {
		pMap->loadMap(mapCode);
		pMap->autorelease();
		return pMap;
	}

	DeleteSafe(pMap);
	return nullptr;
}

SGMapLayer::SGMapLayer()
	: m_pMapInfo(nullptr)
	, m_pActorBox(nullptr)
{}

SGMapLayer::~SGMapLayer() {
}

bool SGMapLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	m_pActorBox = SGActorBox::getInstance();
	m_pActorBox->init(this);

	

	// =================================================
	// 임시 데이터 주입
	// =================================================

	SGCharacterInfo info;
	SGDataManager* pConfig = SGDataManager::getInstance();
	SGCharBaseInfo* pBaseInfo = pConfig->getCharBaseInfo(CharType::Gunner);
	
	for (int i = 0; i < VisualType::Max; ++i) {
		info.VisualInfo.NpkIndex[i] = pBaseInfo->DefaultVisualNpkIndex[i];
		info.VisualInfo.ImgIndex[i] = pBaseInfo->DefaultVisualImgIndex[i];
	}

	SGPlayer* pPlayer = SGPlayer::getInstance();
	pPlayer->setCharacter(m_pActorBox->createCharacterOnMap(CharType::Gunner, 300, 250, info));
	pPlayer->initActionManager();
	pPlayer->initController();
	pPlayer->runBaseAction(BaseAction::Idle);
	pPlayer->setMapLayer(this);

	m_pActorBox->createMonsterOnMap(2, 1, 600, 350);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SGMapLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(SGMapLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}



void SGMapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	SGPlayer::getInstance()->onKeyPressed(keyCode, event);

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		SGGlobal::getInstance()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		SGGlobal::getInstance()->toggleDrawThicknessBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F3)
		SGGlobal::getInstance()->toggleDrawAttackBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F4)
		SGGlobal::getInstance()->toggleDrawEffect();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F9) {
		SGPlayer::getInstance()->setCharacter(nullptr);
		SGPlayer::getInstance()->setMapLayer(nullptr);
		SGActorBox::getInstance()->clearAll();
		SGDirector::getInstance()->replaceScene(GameScene::createScene());
	}
}

void SGMapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	SGPlayer::getInstance()->onKeyReleased(keyCode, event);
}

SGMapInfo* SGMapLayer::getMapInfo() {
	return m_pMapInfo;
}


void SGMapLayer::update(float dt) {
	SGPlayer::getInstance()->update(dt);

	m_pActorBox->update(dt);
}

void SGMapLayer::onExitTransitionDidStart() {
	Layer::onExitTransitionDidStart();
}


void SGMapLayer::loadMap(int mapCode) {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
	SGMapInfo* pMap = pDataManager->getMapInfo(mapCode);

	m_pMapInfo = pMap;

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = pMap->TileHeight - 1, k = 0; i >= 0; --i, ++k) {	
		for (int j = 0; j < pMap->TileWidth; j++) {
			float fTileXPos = float(TileWidth_v) * j;
			float fTileYPos = float(TileHeight_v) * k;

			SGTileInfo* pTileInfo = pDataManager->getTileInfo(pMap->TileArray[i][j]);
			SGFrameTexture* pFrameTexture = pPackManager->getPack(pTileInfo->NpkIndex)->createFrameTexture(pTileInfo->ImgIndex, pTileInfo->SpriteIndex);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->getTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(fTileXPos, fTileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < pMap->ObstacleList.Size(); ++i) {
		SGMapObjectInfo& objInfo = pMap->ObstacleList[i];
		m_pActorBox->createObstacleOnMap(objInfo.Code, objInfo.X, objInfo.Y);
	}

	// NPC 로딩
}


void SGMapLayer::runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId) {

	if (frameEventType == FrameEventType::Projectile)
		m_pActorBox->createProejctileOnMap(runner, frameEventId);
	else if (frameEventType == FrameEventType::AttackBox)
		int a; // TODO: 어택박스 구현
}


SGCharacter* SGMapLayer::findNearestCharacterInRadious(SGActor* stdActor, float radious, float& enemyDist) {

	int iAllyFlag = stdActor->getAllyFlag();
	SGVec2 stdPos = stdActor->getPositionRealCenter();
	SGCharacter* character = nullptr;
	float minDist = FLT_MAX;
	CharacterList& characterList = m_pActorBox->getCharacterList();

	for (int i = 0; i < characterList.Size(); ++i) {
		auto pCharacter = characterList[i];

		if (pCharacter == stdActor)
			continue;

		float dist = stdPos.distance(pCharacter->getPositionRealCenter());

		if (dist < radious && pCharacter->getAllyFlag() != iAllyFlag && dist < minDist) {
			minDist = dist;
			character = pCharacter;
		}
	}
	return character;
}

bool SGMapLayer::collectEnemiesInActorRect(
	SGActor* attacker,
	const SGActorRect& absoluteActorRect,
	Out_ SGVector<SGHitInfo>& hitTargets) {

	PhysicsActorList& physcisActorList = m_pActorBox->getPhysicsActorList();
	bool bFind = false;

	for (int i = 0; i < physcisActorList.Size(); ++i) {
		auto pHitTarget = physcisActorList[i];	// 공격받을 대상
		SGRect hitRect;
		SpriteDirection_t eHitDirection;

		if (pHitTarget->getAllyFlag() == attacker->getAllyFlag())
			continue;

		// 몬스터 기준으로 플레이어 충돌이라
		// eHitDirection은 플레이어의 충돌방향이 되므로, 반대로 돌려줘야함.
		if (pHitTarget->isCollide(absoluteActorRect, eHitDirection, hitRect)) {
			hitTargets.PushBack({ attacker, pHitTarget, SpriteDirection::Reverse[eHitDirection], hitRect, nullptr });
			bFind = true;
		}
	}

	return bFind;
}


bool SGMapLayer::collectEnemiesInActor(SGActor* collector, Out_ SGVector<SGHitInfo>& hitTargets) {
	return collectEnemiesInActorRect(collector, collector->getActorRect(), hitTargets);
}


bool SGMapLayer::isCollideWithObstacles(const SGRect& rect) {

	ObstacleList& collidableObstacles = m_pActorBox->getCollidableObstacleList();

	for (int i = 0; i < collidableObstacles.Size(); ++i) {
		SGRect thicknessBox = collidableObstacles[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(rect))
			return true;
	}

	return false;
}

