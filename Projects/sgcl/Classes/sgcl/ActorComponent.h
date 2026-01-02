/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:17:56 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/IUpdatable.h>
#include <sgcl/IComponent.h>

#include <sgcl/Actor.h>

class ActorComponent : public IComponent
{
public:
	ActorComponent(Actor* _pActor)
	: pActor_(_pActor)
	{
	}

	Actor* GetActor() const { return pActor_; }

protected:
	Actor* pActor_;
};
