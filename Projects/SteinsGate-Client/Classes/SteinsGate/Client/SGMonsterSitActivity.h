/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:17 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGMonsterActivity.h>

class SGMonsterSitActivity : public SGMonsterActivity
{
public:
	SGMonsterSitActivity(SGMonster* monster);

	void onActivityBegin() override;
	void onActivityEnd() override;

	void onUpdate(float dt) override;
};


