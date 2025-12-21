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

	static MapLayer* Create();

	void LoadMap(int _mapCode);
	bool init() override;

	void update(float _dt) override;

	Character* FindNearestCharacterInRadious(Actor* _pStdActor, float _radious, JCORE_OUT float& _enemyDist);
	bool CollectEnemiesInActorRect(Actor* _pAttacker, const ActorRect& _rect, JCORE_OUT SGVector<HitInfo>& _hitTargets);
	bool CollectEnemiesInActor(Actor* _pCollector, JCORE_OUT SGVector<HitInfo>& _hitTargets);
	bool IsCollideWithMapObjects(const SGRect& _rect) const;

	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) override;

	MapPhysicsInfo* GetMapPhysicsInfo() const;
	MapAreaInfo* GetMapAreaInfo() const;
	MapInfo* GetMapInfo() const;
	MimicCamera* GetCamera() const;

private:
	bool isMapLoaded_;

	MapInfo*			pMapInfo_ = nullptr;
	MimicCamera*		pCamera_ = nullptr;
	MapPhysicsInfo*		pMapPhysicsInfo_ = nullptr;
	MapAreaInfo*		pMapAreaInfo_ = nullptr;
	ActorManager*		pActorBox_ = nullptr;
};
