/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGMonsterActivity.h>

class SGMonsterFallDownActivity : public SGMonsterActivity
{
public:
	SGMonsterFallDownActivity(SGMonster* monster);

	void onActivityBegin() override;
	void onActivityEnd() override;
	void onUpdate(float dt) override;
private:
	bool m_bBounced;
	bool m_bDown;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};




