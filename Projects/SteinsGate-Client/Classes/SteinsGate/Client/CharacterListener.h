/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ActorListener.h>
#include <SteinsGate/Client/Struct.h>

class Character;
class JCORE_NOVTABLE CharacterListener : public ActorListener
{
public:
	CharacterListener() : m_pCharacter(nullptr) {}

	void onCleanUp() override;
	void injectActor(Actor* actor) override;

	virtual void onCollisionWithGround() {}
	virtual void onLifeTimeOver() {}
	virtual void onDistanceOver() {}
	Type getListenerType() const override { return eCharacter; }
protected:
	Character* m_pCharacter;
};


