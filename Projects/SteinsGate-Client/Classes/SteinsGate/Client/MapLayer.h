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

	Character* findNearestCharacterInRadious(Actor* stdActor, float radious, JCORE_OUT float& enemyDist);
	bool collectEnemiesInActorRect(Actor* attacker, const ActorRect& rect, JCORE_OUT SGVector<HitInfo>& hitTargets);
	bool collectEnemiesInActor(Actor* collector, JCORE_OUT SGVector<HitInfo>& hitTargets);
	bool isCollideWithMapObjects(const SGRect& rect);

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



