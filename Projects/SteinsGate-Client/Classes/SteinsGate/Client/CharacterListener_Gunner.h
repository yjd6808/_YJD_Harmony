/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/CharacterListener.h>

class CharacterListener_Gunner : public CharacterListener
{
public:
	CharacterListener_Gunner(Character* character) : CharacterListener(character) {}
	SG_CHARACTER_LISTENER_FACTORY(CharacterListener_Gunner)
};


