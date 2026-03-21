/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include "sgcl/IActorListener.h"
#include "sgcl/Monster.h"

#define SG_MONSTER_LISTENER_FACTORY(Type)                              \
public:                                                                \
    struct Factory : IFactory                                          \
    {                                                                  \
        MonsterListener* Create(Monster* _pMonster) override           \
        {                                                              \
            return dbg_new Type(_pMonster);                            \
        }                                                              \
    };

class MonsterListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual MonsterListener* Create(Monster* _pMonster) = 0;
	};

	MonsterListener(Monster* _pMonster)
	: m_pMonster(_pMonster)
	{
	}

	~MonsterListener() override = default;

	Type GetListenerType() const override
	{
		return eMonster;
	}

protected:
	NOT_NULL Monster* m_pMonster;
};
