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

	void initialize() override;
	void onUpdate(float _dt) override;

	void setSpeed(const SGVec2& _speed);
	void setSpeed(float _x, float _y);

	const SGVec2& getSpeed() const;

	SG_COMPONENT_TYPE_GETTER(Type::eMove)

private:
	void updateLeftMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void updateRightMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void updateUpMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);
	void updateDownMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const SGRect& _thicknessRect);

private:
	SGVec2 speed_;
};
