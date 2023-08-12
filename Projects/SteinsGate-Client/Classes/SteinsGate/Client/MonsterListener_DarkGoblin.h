/*
 * 작성자: 윤정도
 * 생성일: 8/11/2023 5:50:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/MonsterListener.h>

class MonsterListener_DarkGoblin : public MonsterListener
{
public:
	MonsterListener_DarkGoblin(Monster* monster) : MonsterListener(monster) {}
	void onCreated() override;
	SG_MONSTER_LISTENER_FACTORY(MonsterListener_DarkGoblin)
};


