/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:17:56 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/Type.h>
#include <SteinsGate/Common/Enum.h>

#include <SteinsGate/Client/IUpdatable.h>
#include <SteinsGate/Client/IComponent.h>

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/Const.h>

class ActorComponent : public IComponent
{
public:
	ActorComponent(Actor* _pActor)
	: pActor_(_pActor)
	{
	}

	Actor* getActor() const { return pActor_; }

protected:
	Actor* pActor_;
};
