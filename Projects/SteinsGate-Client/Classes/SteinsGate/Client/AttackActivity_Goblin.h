/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/AttackActivity.h>

class AttackActivity_Goblin : public AttackActivity
{
public:
	AttackActivity_Goblin(Actor* actor);

	void onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) override;
	void onActivityBegin() override;
	
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onUpdate(float dt) override;
	void onEnemySingleHit(HitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);
};




