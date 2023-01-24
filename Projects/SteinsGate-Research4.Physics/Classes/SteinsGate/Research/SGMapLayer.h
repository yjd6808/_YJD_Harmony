/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Research/Enums.h>
#include <SteinsGate/Research/SGPlayer.h>


class SGActor;
class SGMapLayer : public cocos2d::Layer
{
public:
	SGMapLayer();
	~SGMapLayer() override;

	bool init() override;
	static SGMapLayer* create();

	void update(float dt) override;
	void updateZOrder(float dt);
	void updateActors(float dt);
	void updateCollision(float dt);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	void runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId);
	void createProejctile(SGActor* spawner, int projectileId);
	void createHitbox(SGActor* spawner, int hitBoxId);
	void createMonster(float x, float y, int code);

	void registerZOrderActor(SGActor* actor);

	void unregisterZOrderActor(SGActor* actor);
public:
	SGPlayer* m_pPlayer;
	SGVector<SGActor*> m_vZOrderedActors;	// Z오더를 고유 ID처럼 사용해도 될듯? 이진탐색으로 삭제 함 댈것같다.
	SGSet<SGActor*> m_s;
	SGSet<SGActor*> m_sMyBullets;

	float m_fZReorderTime;
};


