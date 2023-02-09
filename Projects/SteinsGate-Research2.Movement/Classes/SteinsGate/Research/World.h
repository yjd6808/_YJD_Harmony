/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 3:17:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/Player.h>

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
private:
	// 씬
	// 플레이어
	cocos2d::Camera* m_pCamera;
	cocos2d::Director* m_pDirector;
	cocos2d::Scheduler* m_pScheduler;
	cocos2d::EventDispatcher* m_pEventDispatcher;

	Player* m_pPlayer;
	State m_eState;
};


