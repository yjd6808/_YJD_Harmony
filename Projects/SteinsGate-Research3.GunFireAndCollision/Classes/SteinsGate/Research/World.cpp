/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 3:18:11 AM
 * =====================
 *
 */

#include "World.h"


#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Common/Engine/GridLayer.h>


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

	CC_SAFE_RELEASE(m_pWorldLayer);

	for (int i = 0; i < m_vTesters.Size(); ++i) {
		CC_SAFE_RELEASE(m_vTesters[i]);
	}
}


void World::init() {
	m_pDirector = Director::getInstance();
	m_pCamera = Camera::getDefaultCamera();
	m_pScheduler = m_pDirector->getScheduler();
	m_pEventDispatcher = m_pDirector->getEventDispatcher();

	m_pWorldLayer = WorldLayer::create();
	m_pWorldLayer->retain();

	m_pGridLayer = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	m_pGridLayer->setAnchorPoint(Vec2::ZERO);
	m_pGridLayer->setVisible(false);
	m_pWorldLayer->addChild(m_pGridLayer, 0);

	m_pPlayer = Player::create(ColliderType::Character, CharacterType::Gunner);

	for (int i = 0; i < 14; ++i) {
		auto lb= Label::create(cocos2d::StringUtils::format("%d", i).c_str(), "", 16);
		auto test = Collider::create(ColliderType::Obstable, CharacterType::Max);
		test->retain();
		test->addChild(lb);
		m_vTesters.PushBack(test);
		m_vReorderNodes.PushBack(test);
	}

	m_pScheduler->schedule(CC_CALLBACK_1(World::update, this), this, WorldFPS_v, false, "WorldUpdate");

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(World::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(World::onKeyReleased, this);
	m_pEventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 50);
}

void World::addCollider(Collider* col) {
	m_pWorldLayer->addChild(col);
	m_vReorderNodes.PushBack(col);
}

// 임시
// 나중에는 월드 업데이트 이후 수행해야할 이벤트를 처리하는 기능을 따로 만들어야할듯
void World::removeProjectile(Collider* col) {
	m_pWorldLayer->removeChild(col);
	m_vReorderNodes.Remove(col);
}

bool World::isCollide(Collider* collider) {
	for (int i = 0; i < m_vTesters.Size(); ++i) {
		if (m_vTesters[i]->isCollide(collider)) {
			return true;
		}
	}
	return false;
}

bool World::isCollideTarget(Collider* collider, Out_ Collider** target) {
	for (int i = 0; i < m_vTesters.Size(); ++i) {
		if (m_vTesters[i]->isCollide(collider)) {
			*target = m_vTesters[i];
			return true;
		}
	}
	return false;
}

float PosTime = 0.0f;
float ZOrderTime = 0.0f;
void World::update(float delta) {
	if (isSceneLoading())
		return;

	ZOrderTime += delta;

	//PosTime += delta;
	if (PosTime >= 3.0f) {
		//PosTime = 0.0f;
		Rect thickBox = m_pPlayer->getThicknessBox();
		Rect hitBox = m_pPlayer->getHitBox();
		Log("플레이어 두께박스 (x, y): %d, %d / (width, height)): %d, %d\n", 
			(int)thickBox.origin.x, int(thickBox.origin.y), int(thickBox.size.width), int(thickBox.size.height));
		Log("플레이어 히트박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			(int)hitBox.origin.x, int(hitBox.origin.y), int(hitBox.size.width), int(hitBox.size.height));
	}

	if (m_pRunningScene == nullptr)
		return;


	if (ZOrderTime >= 0.1f) {
		ZOrderTime = 0.0f;
		auto playerThickBox = m_pPlayer->getThicknessBox();

		m_vReorderNodes.Sort([](Collider* node1, Collider* node2) {
			auto box1 = node1->getThicknessBox();
			auto box2 = node2->getThicknessBox();
			return box1.getMidY() > box2.getMidY();
		});

		for (int i = 0, iOrder = 1; i < m_vReorderNodes.Size(); ++i) {
			m_pWorldLayer->reorderChild(m_vReorderNodes[i], iOrder++);
		}
		
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




	int zorderIdx = 0;


	// 절반은 뛰엄뛰엄
	for (int i = 0; i < m_vTesters.Size() / 2; ++i) {
		m_vTesters[i]->setRealPos(200.0f * i, 50.0f * i);
		m_pWorldLayer->addChild(m_vTesters[i], ++zorderIdx);
		Rect thickBox = m_vTesters[i]->getThicknessBox();
		Rect hitBox = m_vTesters[i]->getHitBox();
		Log("%d번 테스트 오브젝트 ZOrder: %d\n", i, m_vTesters[i]->getLocalZOrder());
		Log("├ 두께박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			i, (int)thickBox.origin.x, int(thickBox.origin.y), int(thickBox.size.width), int(thickBox.size.height));
		Log("└ 히트박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			i, (int)hitBox.origin.x, int(hitBox.origin.y), int(hitBox.size.width), int(hitBox.size.height));

	}


	// 절반은 따닥따닥
	for (int i = m_vTesters.Size() / 2; i < m_vTesters.Size(); ++i) {
		m_vTesters[i]->setRealPos(50.0f + 20.0f * (i - m_vTesters.Size() / 2), 400.0f + 15.0f * (i - m_vTesters.Size() / 2));
		m_pWorldLayer->addChild(m_vTesters[i], ++zorderIdx);
		Rect thickBox = m_vTesters[i]->getThicknessBox();
		Rect hitBox = m_vTesters[i]->getHitBox();

		Log("%d번 테스트 오브젝트 ZOrder: %d\n", i, m_vTesters[i]->getLocalZOrder());
		Log("├ 두께박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			(int)thickBox.origin.x, int(thickBox.origin.y), int(thickBox.size.width), int(thickBox.size.height));
		Log("└ 히트박스 (x, y): %d, %d / (width, height)): %d, %d\n",
			(int)hitBox.origin.x, int(hitBox.origin.y), int(hitBox.size.width), int(hitBox.size.height));
	}

	m_pRunningScene = scene;
	m_pWorldLayer->addChild(m_pPlayer, ++zorderIdx);
	m_pRunningScene->addChild(m_pWorldLayer);
}

