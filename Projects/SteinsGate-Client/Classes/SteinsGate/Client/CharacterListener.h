/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Character.h>

#define SG_CHARACTER_LISTENER_FACTORY(Type)                            \
public:                                                                \
    struct Factory : IFactory                                          \
    {                                                                  \
        CharacterListener* Create(Character* _pChar) override          \
        {                                                              \
            return dbg_new Type(_pChar);                               \
        }                                                              \
    };

class CharacterListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual CharacterListener* Create(Character* _pChar) = 0;
	};

	CharacterListener(Character* _pChar)
	: pChar_(_pChar)
	{
	}

	void OnCleanUp() override;

	Type GetListenerType() const override
	{
		return eCharacter;
	}

protected:
	NOT_NULL Character* pChar_;
};
