/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Common/MapInfo.h>
#include <SteinsGate/Common/MapPhysicsInfo.h>

#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/MimicCamera.h>

class MapLayer : public SGLayer
{
public:
	MapLayer();
	~MapLayer() override;

	static MapLayer* create();

	void loadMap(int _mapCode);
	bool init() override;

	void update(float _dt) override;

	Character* findNearestCharacterInRadious(Actor* _pStdActor, float _radious, JCORE_OUT float& _enemyDist);
	bool collectEnemiesInActorRect(Actor* _pAttacker, const ActorRect& _rect, JCORE_OUT SGVector<HitInfo>& _hitTargets);
	bool collectEnemiesInActor(Actor* _pCollector, JCORE_OUT SGVector<HitInfo>& _hitTargets);
	bool isCollideWithMapObjects(const SGRect& _rect) const;

	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	MapPhysicsInfo* getMapPhysicsInfo() const;
	MapAreaInfo* getMapAreaInfo() const;
	MapInfo* getMapInfo() const;
	MimicCamera* getCamera() const;

public:
	bool m_bMapLoaded;

	MimicCamera* m_pCamera;

	MapPhysicsInfo* m_pMapPhysicsInfo;
	MapAreaInfo* m_pMapAreaInfo;
	MapInfo* m_pMapInfo;
	ActorManager* m_pActorBox;
};
