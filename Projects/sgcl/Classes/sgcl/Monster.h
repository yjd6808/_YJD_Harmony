/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once


#include "sg/_Struct/SteinsGate_MonsterStat.h"

#include "sgcl/Actor.h"
#include "sg/_Struct/SteinsGate_Monster.h"
#include "sg/_Struct/SteinsGate_AI.h"

class Monster : public Actor
{
public:
	Monster(MonsterInfo* _pBaseInfo);
	~Monster() override;

	static Monster* Create(MonsterInfo* _pBaseInfo);

	void Initialize() override;
	void InitActorSprite() override;
	void InitListeners() override;
	void InitComponents() override;

	void Hit(const HitInfo& _hitInfo) override;

	void SetStatInfo(MonsterStatInfo* _pStatInfo);

	ActorType_t GetType() const override { return ActorType::Monster; }
	MonsterInfo* GetBaseInfo();
	MonsterStatInfo* GetStatInfo();

	int GetCode() override { return pBaseInfo_->code_; }

private:
	MonsterInfo* pBaseInfo_;
	MonsterStatInfo* pStatInfo_;
};
