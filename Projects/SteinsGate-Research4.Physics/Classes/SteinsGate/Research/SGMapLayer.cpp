/*
 * 작성자 : 윤정도
 */

#include "SGMapLayer.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGProjectile.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGMapLayer* SGMapLayer::create() {
	SGMapLayer* pWorld = new (std::nothrow) SGMapLayer;

	if (pWorld && pWorld->init()) {
		pWorld->autorelease();
		return pWorld;
	}

	DeleteSafe(pWorld);
	return nullptr;
}


SGMapLayer::SGMapLayer()
	: m_fZReorderTime(0.0f)
	, m_pPlayer(nullptr)
{}

SGMapLayer::~SGMapLayer() {
	DeleteSafe(m_pPlayer); // 임시
}

// 임시
SGPlayer* MainPlayer_v = nullptr;


bool SGMapLayer::init() {

	if (!Layer::init()) {
		return false;
	}


	// =================================================
	// 임시 데이터 주입
	// =================================================
	SGCharacterInfo info;
	SGDataManager* pConfig = SGDataManager::getInstance();
	SGCharacterBaseInfo* pBaseInfo = pConfig->getCharacterBaseInfo(CharacterType::Gunner);
	
	for (int i = 0; i < VisualType::Max; ++i) {
		info.VisualInfo.NpkIndex[i] = pBaseInfo->DefaultVisualNpkIndex[i];
		info.VisualInfo.ImgIndex[i] = pBaseInfo->DefaultVisualImgIndex[i];
	}

	SGActionInfo* pIdle = pConfig->getActionInfo("idle");
	SGActionInfo* pWalk = pConfig->getActionInfo("walk");
	SGActionInfo* pRun = pConfig->getActionInfo("run");
	SGActionInfo* pSliding = pConfig->getActionInfo("sliding");
	SGActionInfo* pGunShot = pConfig->getActionInfo("gun_shot");

	info.ValidAction.PushBack(pIdle->Code);
	info.ValidAction.PushBack(pWalk->Code);
	info.ValidAction.PushBack(pRun->Code);
	info.ValidAction.PushBack(pSliding->Code);
	info.ValidAction.PushBack(pGunShot->Code);

	SGCharacter* pPlayerCharacter = SGCharacter::create(CharacterType::Gunner, info);
	pPlayerCharacter->retain();
	pPlayerCharacter->setPosition(Vec2{ 300, 300 });
	
	m_pPlayer = new SGPlayer();
	MainPlayer_v = m_pPlayer;
	m_pPlayer->setCharacter(pPlayerCharacter);
	m_pPlayer->initActionManager();
	m_pPlayer->initController();
	m_pPlayer->runBaseAction(BaseAction::Idle);
	m_pPlayer->setMapLayer(this);
	registerZOrderActor(pPlayerCharacter);


	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SGMapLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(SGMapLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}



void SGMapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pPlayer->onKeyPressed(keyCode, event);

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		SGGlobal::getInstance()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		SGGlobal::getInstance()->toggleDrawThicknessBox();
}

void SGMapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pPlayer->onKeyReleased(keyCode, event);
}

void SGMapLayer::runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId) {

	if (frameEventType == FrameEventType::Projectile)
		createProejctile(runner, frameEventId);
	else if (frameEventType == FrameEventType::Hitbox)
		createHitbox(runner, frameEventId);

}

void SGMapLayer::createProejctile(SGActor* spawner, int projectileId) {
	SGProjectileInfo* info = SGDataManager::getInstance()->getProjectileInfo(projectileId);
	SGProjectile* projectile = SGProjectile::create(spawner, info);
	registerZOrderActor(projectile);
}

void SGMapLayer::createHitbox(SGActor* spawner, int hitBoxId) {
	
}

void SGMapLayer::registerZOrderActor(SGActor* actor) {
	m_vZOrderedActors.PushBack(actor);
	this->addChild(actor);
}

void SGMapLayer::unregisterZOrderActor(SGActor* actor) {

}

void SGMapLayer::update(float dt) {
	m_pPlayer->update(dt);

	updateZOrder(dt);
	updateActors(dt);
}

void SGMapLayer::updateZOrder(float dt) {

	m_fZReorderTime += dt;
	// 0.1초 정도마다 재정렬 진행해주자.
	if (m_fZReorderTime < 0.1f) {
		return;
	}
	m_fZReorderTime = 0.0f;

	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	for (int i = 1; i < m_vZOrderedActors.Size(); ++i) {
		int find = i;
		for (int j = i - 1; j >= 0; --j) {

			if (m_vZOrderedActors[j]->getThicknessBoxRect().getMidY() > 
				m_vZOrderedActors[find]->getThicknessBoxRect().getMidY()) 
			{
				CC_SWAP(m_vZOrderedActors[j], m_vZOrderedActors[find], SGActor*);
				find = j;
			}
			else {
				// 위치를 찾은 경우 바로 반환되므로 최선의 경우(대부분 정렬된 상태인 경우)에는
				// 대부분 j반복문 진입하자마자 break로 나가버림
				break;
			}
		}
	}


	for (int iOrder = 1; iOrder < m_vZOrderedActors.Size(); ++iOrder) {
		reorderChild(m_vZOrderedActors[iOrder - 1], iOrder);
	}
}

void SGMapLayer::updateActors(float dt) {
	for (int i = 0; i < m_vZOrderedActors.Size(); ++i) {
		m_vZOrderedActors[i]->update(dt);
	}
}


void SGMapLayer::updateCollision(float dt) {
	
}

