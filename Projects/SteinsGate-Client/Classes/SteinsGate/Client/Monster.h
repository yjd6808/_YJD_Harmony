/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/MonsterStatInfo.h>

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/MonsterInfo.h>
#include <SteinsGate/Client/AIInfo.h>

class Monster : public Actor
{
public:
	Monster(MonsterInfo* _pBaseInfo);
	~Monster() override;

	static Monster* create(MonsterInfo* _pBaseInfo);

	void initialize() override;
	void initActorSprite() override;
	void initListeners() override;
	void initComponents() override;

	void hit(const HitInfo& _hitInfo) override;

	void setStatInfo(MonsterStatInfo* _pStatInfo);

	ActorType_t getType() const override { return ActorType::Monster; }
	MonsterInfo* getBaseInfo();
	MonsterStatInfo* getStatInfo();

	int getCode() override { return baseInfo_->code_; }

private:
	MonsterInfo* baseInfo_;
	MonsterStatInfo* statInfo_;
};
