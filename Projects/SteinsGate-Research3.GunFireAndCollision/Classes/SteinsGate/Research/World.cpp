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
	if (m_pPlayer->getReferenceCount() == 1)
		Log("플레이어 안전하게 제거 완료\n");

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

float PosTime = 0.0f;

void World::update(float delta) {
	if (isSceneLoading())
		return;

	m_pPlayer->update(delta);

	PosTime += delta;
	if (PosTime >= 3.0f) {
		//PosTime = 0.0f;
		Rect thickBox = m_pPlayer->getThicknessBox();
		Rect hitBox = m_pPlayer->getHitBox();
		Log("플레이어 두께박스 (x, y): %d, %d / (width, height)): %d, %d\n", 
			(int)thickBox.origin.x, int(thickBox.origin.y), int(thickBox.size.width), int(thickBox.size.height));
		Log("플레이어 히트박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			(int)hitBox.origin.x, int(hitBox.origin.y), int(hitBox.size.width), int(hitBox.size.height));
	}
	
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
	m_pPlayer->setRealPos(200.0f, 200.0f);
	scene->addChild(m_pPlayer);
}


