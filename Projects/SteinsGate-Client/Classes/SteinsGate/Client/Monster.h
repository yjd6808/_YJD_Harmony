/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Common/MonsterStatInfo.h>

#include <SteinsGate/Client/AIActor.h>
#include <SteinsGate/Client/MonsterInfo.h>
#include <SteinsGate/Client/AIInfo.h>

class Monster : public AIActor
{
public:
	Monster(MonsterInfo* baseInfo, AIInfo* aiInfo);
	~Monster() override;

	static Monster* create(MonsterInfo* baseInfo, AIInfo* aiInfo);
	void initActorSprite() override;
	void initAIActivities() override;
	void initComponents() override;
	void hit(const HitInfo& hitInfo) override;

	void update(float dt) override;

	void initWanderAcitivity(AIActivity* wanderActivity);
	void initTrackAcitivity(AIActivity* trackActivity);
	void initAngryAcitivity(AIActivity* angryActivity);

	void onSelectedActivity(AIActivity* selectedActivity) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	void setStatInfo(MonsterStatInfo* statInfo);

	MonsterInfo* getBaseInfo();
	MonsterStatInfo* getStatInfo();

	int getCode() override { return m_pBaseInfo->Code; }
private:
	MonsterInfo* m_pBaseInfo;
	MonsterStatInfo* m_pStatInfo;
};


