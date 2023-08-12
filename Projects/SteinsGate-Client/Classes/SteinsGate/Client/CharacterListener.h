/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Character.h>

#define SG_CHARACTER_LISTENER_FACTORY(Type)								\
public:																	\
	struct Factory : IFactory {											\
		CharacterListener* create(Character* character) override {		\
			return dbg_new Type(character);								\
		}																\
	};

class CharacterListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual CharacterListener* create(Character* character) = 0;
	};

	CharacterListener(Character* character) : m_pCharacter(character) {}

	void onCleanUp() override;

	Type getListenerType() const override { return eCharacter; }
protected:
	JCORE_NOT_NULL Character* m_pCharacter;
};


