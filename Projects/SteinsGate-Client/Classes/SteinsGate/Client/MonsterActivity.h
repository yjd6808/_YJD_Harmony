/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:36:30 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/AIActivity.h>

class Monster;
class MonsterActivity : public AIActivity {
public:
	MonsterActivity(Monster* monster, AIActivityType_t type);
	~MonsterActivity() override = default;

	void run() override;
	void stop() override;

	AIActor* getAIActor() override;
protected:
	Monster* m_pMonster;
};


