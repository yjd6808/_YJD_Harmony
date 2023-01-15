/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 3:18:11 AM
 * =====================
 *
 */

#include "World.h"


#include <SteinsGate/Research/Config.h>



using namespace cocos2d;

World* World::getInstance() {
	static World* pWorld;;

	if (pWorld == nullptr) {
		pWorld = new World;
		pWorld->init();
		return pWorld;
	}

	return pWorld;
}

World::~World() {
	CC_SAFE_RELEASE(m_pPlayer);
}

void World::init() {
	m_pDirector = Director::getInstance();
	m_pCamera = Camera::getDefaultCamera();
	m_pScheduler = m_pDirector->getScheduler();
	m_pEventDispatcher = m_pDirector->getEventDispatcher();

	m_pPlayer = Player::create();
	m_pPlayer->retain();
	
	m_pScheduler->schedule(CC_CALLBACK_1(World::update, this), this, WorldFPS_v, false, "WorldUpdate");

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(World::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(World::onKeyReleased, this);
	m_pEventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 50);
}

void World::update(float delta) {
	if (isSceneLoading())
		return;

	m_pPlayer->update(delta);
}

void World::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (isSceneLoading())
		return;


	m_pPlayer->onKeyPressed(keyCode, event);
}


void World::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pPlayer->onKeyReleased(keyCode, event);
}

void World::onSceneLoaded(cocos2d::Scene* scene) {

	// 씬로딩 되면 캐릭터 위치랑
	// 전반적으로 업데이트 해줘야함

	m_eState = Playing;
	m_pPlayer->setPosition(100, 100);
	scene->addChild(m_pPlayer);
}


