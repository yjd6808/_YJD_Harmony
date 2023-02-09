/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 3:17:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Player.h>
#include <SteinsGate/Research/WorldLayer.h>

#include <SteinsGate/Common/Engine/GridLayer.h>



class World
{
public:
	enum State
	{
		SceneLoading,
		Playing,
	};

	static World* getInstance();
	~World();

	void update(float delta);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onSceneLoaded(cocos2d::Scene* scene);

	void init();
	void setWorldState(State state) { m_eState = state; }
	bool isSceneLoading() { return m_eState == State::SceneLoading; }
	Player* getPlayer() { return m_pPlayer; }
	void addCollider(Collider* col);
	void removeProjectile(Collider* col);
	bool isCollide(Collider* collider);
	bool isCollideTarget(Collider* collider, Out_ Collider** target);
private:
	cocos2d::Camera* m_pCamera;
	cocos2d::Director* m_pDirector;
	cocos2d::Scheduler* m_pScheduler;
	cocos2d::EventDispatcher* m_pEventDispatcher;
	cocos2d::Scene* m_pRunningScene;

	Player* m_pPlayer;
	WorldLayer* m_pWorldLayer;
	GridLayer* m_pGridLayer;

	JCore::Vector<Collider*> m_vTesters;
	JCore::Vector<Collider*> m_vReorderNodes;
	State m_eState;
	
};


