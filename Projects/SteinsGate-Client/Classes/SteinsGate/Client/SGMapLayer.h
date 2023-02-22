/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGMapInfo.h>
#include <SteinsGate/Client/SGCamera.h>

class SGMapLayer : public SGLayer
{
public:
	SGMapLayer();
	~SGMapLayer() override;

	static SGMapLayer* create();

	void loadMap(int mapCode);
	bool init() override;

	void update(float dt) override;

	void runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId);
	SGCharacter* findNearestCharacterInRadious(SGActor* stdActor, float radious, Out_ float& enemyDist);
	bool collectEnemiesInActorRect(SGActor* attacker, const SGActorRect& rect, Out_ SGVector<SGHitInfo>& hitTargets);
	bool collectEnemiesInActor(SGActor* collector, Out_ SGVector<SGHitInfo>& hitTargets);
	bool isCollideWithObstacles(const SGRect& rect);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

	SGMapInfo* getMapInfo();
	SGCamera* getCamera();
public:
	bool m_bMapLoaded;

	SGCamera* m_pCamera;
	SGMapInfo* m_pMapInfo;
	SGActorBox* m_pActorBox;
};



