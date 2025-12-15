#pragma once

#include <SteinsGate/Client/CharacterListener.h>

class CharacterListener_Gunner : public CharacterListener
{
public:
	CharacterListener_Gunner(Character* _pCharacter);
	SG_CHARACTER_LISTENER_FACTORY(CharacterListener_Gunner)
};
