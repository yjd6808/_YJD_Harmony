/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Character.h>

class JCORE_NOVTABLE CharacterListener : public IActorListener
{
public:
	CharacterListener() : m_pCharacter(nullptr) {}

	void onCleanUp() override;

	virtual void onCollisionWithGround() {}
	virtual void onLifeTimeOver() {}
	virtual void onDistanceOver() {}

	Type getListenerType() const override { return eCharacter; }

	void setActor(Actor* actor) override;
	Actor* getActor() override { return m_pCharacter; }
	
protected:
	Character* m_pCharacter;
};


