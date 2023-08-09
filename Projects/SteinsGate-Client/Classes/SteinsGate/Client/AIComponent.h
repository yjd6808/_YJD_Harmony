/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:08:04 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/ActorComponent.h>

class AIComponent : public ActorComponent, public IUpdatable
{
public:
	AIComponent(Actor* actor);

	void onUpdate(float dt) override;

	SG_COMPONENT_TYPE_GETTER(Type::eAI)
private:
};

