/*
 * 작성자 : 윤정도
 */

#include "WorldLayer.h"

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <JCore/Utils/Console.h>

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

WorldLayer* WorldLayer::create() {
	WorldLayer* pWorld = new (std::nothrow) WorldLayer;

	if (pWorld && pWorld->init()) {
		pWorld->autorelease();
		return pWorld;
	}

	DeleteSafe(pWorld);
	return nullptr;
}

WorldLayer::~WorldLayer() {
	DeleteSafe(m_pPlayer); // 임시
}

bool WorldLayer::init() {

	if (!Layer::init()) {
		return false;
	}


	// =================================================
	// 임시 데이터 주입
	// =================================================
	SGCharacterInfo info;
	SGConfigManager* pConfig = SGConfigManager::getInstance();
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
	this->addChild(pPlayerCharacter);

	m_pPlayer = new SGPlayer();
	m_pPlayer->setCharacter(pPlayerCharacter);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(WorldLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(WorldLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}



void WorldLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pPlayer->onKeyPressed(keyCode, event);
}

void WorldLayer::update(float dt) {
	m_pPlayer->update(dt);
}

