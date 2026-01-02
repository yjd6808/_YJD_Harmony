/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */

#pragma once


#include <sgcl/AttackActivity.h>

class AttackActivity_Goblin : public AttackActivity
{
public:
	AttackActivity_Goblin(Actor* _pActor);

	void OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState) override;
	void OnActivityBegin() override;

	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnUpdate(float _dt) override;

	void OnEnemySingleHit(HitInfo& _info);
	void OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);
};
