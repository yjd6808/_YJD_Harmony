/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:36:30 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGAIActivity.h>

class SGMonster;
class SGMonsterActivity : public SGAIActivity {
public:
	SGMonsterActivity(SGMonster* monster, AIActivity_t type);
	~SGMonsterActivity() override = default;

	void run() override;
	void stop() override;

	SGAIActor* getAIActor() override;
protected:
	SGMonster* m_pMonster;
};


