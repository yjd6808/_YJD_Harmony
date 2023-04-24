/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/MonsterActivity.h>

class HitRecorder;
class MonsterHitActivity : public MonsterActivity
{
public:
	MonsterHitActivity(Monster* actor);

	void selectHitAnimation();
	void checkPosition();
	void onActivityBegin() override;
	void updateGroundHitState(float dt);
	void updateDownState(float dt);
	void updateAirHitState(float dt);
	void onUpdate(float dt) override;
private:
	bool m_bHitSmall;
	bool m_bOnTheGround;		// 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
	bool m_bDownTimeCheckBegin;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};

