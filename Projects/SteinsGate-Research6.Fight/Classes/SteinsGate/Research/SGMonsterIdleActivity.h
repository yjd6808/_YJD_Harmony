/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGMonsterActivity.h>

class SGMonsterIdleActivity : public SGMonsterActivity
{
public:
	SGMonsterIdleActivity(SGMonster* monster);

	void onUpdate(float dt) override;
	void onActivityBegin() override;
	void onActivityEnd() override;
};




