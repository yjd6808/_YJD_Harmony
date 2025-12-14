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
	AttackActivity_Goblin(Actor* _pActor);

	void onActivitySelectFromAIRoutine(AIInfo* _pAiInfo, AIState_t _aiState) override;
	void onActivityBegin() override;
	
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void onUpdate(float _dt) override;
	void onEnemySingleHit(HitInfo& _info);
	void onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);
};



