/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MonsterActivity.h>

class MonsterFallDownActivity : public MonsterActivity
{
public:
	MonsterFallDownActivity(Monster* monster);

	void onActivityBegin() override;
	void onActivityEnd() override;
	void onUpdate(float dt) override;
private:
	bool m_bBounced;
	bool m_bDown;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};




