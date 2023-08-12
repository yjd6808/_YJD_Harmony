/*
 * 작성자: 윤정도
 * 생성일: 8/11/2023 5:50:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/MonsterListener.h>

class MonsterListener_NormalGoblin : public MonsterListener
{
public:
	MonsterListener_NormalGoblin(Monster* monster) : MonsterListener(monster) {}
	void onCreated() override;
	SG_MONSTER_LISTENER_FACTORY(MonsterListener_NormalGoblin)
};


