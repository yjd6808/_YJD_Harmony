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
	MoveComponent(Actor* actor);

	void initialize() override;
	void onUpdate(float dt) override;

	void setSpeed(const SGVec2& speed);
	void setSpeed(float x, float y);

	const SGVec2& getSpeed() const;

	SG_COMPONENT_TYPE_GETTER(Type::eMove)
private:
	void updateLeftMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect);
	void updateRightMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& nextThicknessRect);
	void updateUpMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& nextThicknessRect);
	void updateDownMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& nextThicknessRect);
private:
	SGVec2 m_Speed;
};


