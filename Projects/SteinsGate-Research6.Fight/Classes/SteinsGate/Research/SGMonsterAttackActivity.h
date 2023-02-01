/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGMonsterActivity.h>

class SGMonsterAttackActivity : public SGMonsterActivity
{
public:
	SGMonsterAttackActivity(SGMonster* monster);

	void onActivityBegin() override;
	
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onUpdate(float dt) override;
	void onEnemySingleHit(SGHitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);
private:
	SGHitRecorder* m_pHitRecorder;
};




