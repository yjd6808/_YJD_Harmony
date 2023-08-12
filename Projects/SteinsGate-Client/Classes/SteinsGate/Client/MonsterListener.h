/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Monster.h>

#define SG_MONSTER_LISTENER_FACTORY(Type)								\
public:																	\
	struct Factory : IFactory {											\
		MonsterListener* create(Monster* monster) override {			\
			return dbg_new Type(monster);								\
		}																\
	};

class MonsterListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual MonsterListener* create(Monster* monster) = 0;
	};

	MonsterListener(Monster* monster) : m_pMonster(monster) {}
	~MonsterListener() override = default;

	Type getListenerType() const override { return eMonster; }
protected:
	JCORE_NOT_NULL Monster* m_pMonster;
};


