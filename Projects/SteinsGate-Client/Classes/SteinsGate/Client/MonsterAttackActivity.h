/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MonsterActivity.h>

class MonsterAttackActivity : public MonsterActivity
{
public:
	MonsterAttackActivity(Monster* monster);

	void onActivityBegin() override;
	
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onUpdate(float dt) override;
	void onEnemySingleHit(SGHitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);
private:
	HitRecorder* m_pHitRecorder;
};




