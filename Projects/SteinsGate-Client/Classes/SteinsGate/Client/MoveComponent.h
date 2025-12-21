/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:23:03 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ActorComponent.h>

struct MapAreaInfo;

class MoveComponent : public ActorComponent, public IUpdatable
{
public:
	MoveComponent(Actor* _pActor);

	void Initialize() override;
	void OnUpdate(float _dt) override;

	void SetSpeed(const SGVec2& _speed);
	void SetSpeed(float _x, float _y);

	const SGVec2& GetSpeed() const;

	SG_COMPONENT_TYPE_GETTER(Type::eMove)

private:
	void UpdateLeftMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void UpdateRightMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void UpdateUpMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void UpdateDownMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);

private:
	SGVec2 speed_;
};
