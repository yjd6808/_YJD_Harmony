/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Client/ActorBox.h>
#include <SteinsGate/Client/MapInfo.h>
#include <SteinsGate/Client/MimicCamera.h>

class MapLayer : public SGLayer
{
public:
	MapLayer();
	~MapLayer() override;

	static MapLayer* create();

	void loadMap(int mapCode);
	bool init() override;

	void update(float dt) override;

	void runFrameEvent(Actor* runner, FrameEventType_t frameEventType, int frameEventId);
	Character* findNearestCharacterInRadious(Actor* stdActor, float radious, Out_ float& enemyDist);
	bool collectEnemiesInActorRect(Actor* attacker, const SGActorRect& rect, Out_ SGVector<SGHitInfo>& hitTargets);
	bool collectEnemiesInActor(Actor* collector, Out_ SGVector<SGHitInfo>& hitTargets);
	bool isCollideWithObstacles(const SGRect& rect);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;

	MapInfo* getMapInfo();
	MimicCamera* getCamera();
public:
	bool m_bMapLoaded;

	MimicCamera* m_pCamera;
	MapInfo* m_pMapInfo;
	ActorBox* m_pActorBox;
};



