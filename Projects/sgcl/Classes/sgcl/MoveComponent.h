/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:23:03 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/ActorComponent.h>

struct MapAreaInfo;

class MoveComponent : public ActorComponent, public IUpdatable
{
public:
	MoveComponent(Actor* _pActor);

	void Initialize() override;
	void OnUpdate(float _dt) override;

	void SetSpeed(const c2d::vec2& _speed);
	void SetSpeed(float _x, float _y);

	const c2d::vec2& GetSpeed() const;

	SG_COMPONENT_TYPE_GETTER(Type::eMove)

private:
	void UpdateLeftMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const c2d::rect& _thicknessRect);
	void UpdateRightMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const c2d::rect& _thicknessRect);
	void UpdateUpMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const c2d::rect& _thicknessRect);
	void UpdateDownMove(MapLayer* _pMapLayer, MapAreaInfo* _pAreaInfo, const c2d::rect& _thicknessRect);

private:
	c2d::vec2 speed_;
};
