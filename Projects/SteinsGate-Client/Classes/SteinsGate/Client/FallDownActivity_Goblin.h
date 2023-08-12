/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/FallDownActivity.h>

class FallDownActivity_Goblin : public FallDownActivity
{
public:
	FallDownActivity_Goblin(Actor* actor);

	void onActivityBegin() override;
	void onActivityEnd() override;
	void onUpdate(float dt) override;
private:
	bool m_bBounced;
	bool m_bDown;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};




