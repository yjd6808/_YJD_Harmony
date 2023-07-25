/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Client/AIActor.h>
#include <SteinsGate/Client/MobInfo.h>
#include <SteinsGate/Client/AIInfo.h>

class Monster : public AIActor
{
public:
	Monster(MobInfo* baseInfo, AIInfo* aiInfo);
	~Monster() override;

	static Monster* create(MobInfo* baseInfo, AIInfo* aiInfo);
	void initActorSprite() override;
	void initAIActivities() override;
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

	MobInfo* getBaseInfo();
	int getCode() override { return m_pBaseInfo->Code; }
private:
	MobInfo* m_pBaseInfo;
};


