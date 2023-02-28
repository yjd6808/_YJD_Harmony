﻿/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Client/SGAIActor.h>
#include <SteinsGate/Client/SGMobInfo.h>
#include <SteinsGate/Client/SGAIInfo.h>

class SGMonster : public SGAIActor
{
public:
	SGMonster(SGMobInfo* baseInfo, SGAIInfo* aiInfo);
	~SGMonster() override;

	static SGMonster* create(SGMobInfo* baseInfo, SGAIInfo* aiInfo);
	void initActorSprite() override;
	void initAIActivities() override;
	void initListener(SGActorListener* listener) override;
	void hit(const SGHitInfo& hitInfo) override;

	void update(float dt) override;

	void initWanderAcitivity(SGAIActivity* wanderActivity);
	void initTrackAcitivity(SGAIActivity* trackActivity);
	void initAngryAcitivity(SGAIActivity* angryActivity);

	void onSelectedActivity(SGAIActivity* selectedActivity) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	SGMobInfo* getBaseInfo();
private:
	SGMobInfo* m_pBaseInfo;
};

