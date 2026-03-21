/*
 * 작성자 : 윤정도
 */
#pragma once

#include "sg/_Struct/SteinsGate_Map.h"
#include "sg/_Struct/SteinsGate_MapPhysics.h"

#include "sgcl/ActorManager.h"
#include "sgcl/MimicCamera.h"

class MapLayer : public cc::Layer
{
public:
	MapLayer();
	~MapLayer() override;

	static MapLayer* Create();

	void LoadMap(int _mapCode);
	bool init() override;

	void update(float _dt) override;

	Character* FindNearestCharacterInRadious(Actor* _pStdActor, float _radious, OUT float& _enemyDist);
	bool CollectEnemiesInActorRect(Actor* _pAttacker, const ActorRect& _rect, OUT jc::Vector<HitInfo>& _hitTargets);
	bool CollectEnemiesInActor(Actor* _pCollector, OUT jc::Vector<HitInfo>& _hitTargets);
	bool IsCollideWithMapObjects(const cc::rect& _rect) const;

	void onKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void onKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;

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
