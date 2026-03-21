#pragma once

#include "sgcl/CharacterListener.h"

class CharacterListener_Gunner : public CharacterListener
{
public:
	CharacterListener_Gunner(Character* _pChar);
	SG_CHARACTER_LISTENER_FACTORY(CharacterListener_Gunner)
};
