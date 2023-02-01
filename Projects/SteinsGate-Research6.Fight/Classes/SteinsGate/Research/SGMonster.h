/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGMonsterInfo.h>
#include <SteinsGate/Research/SGAIInfo.h>

class SGMonster : public SGAIActor
{
public:
	SGMonster(SGMonsterInfo* baseInfo, SGMapLayer* mapLayer, SGAIInfo* aiInfo);
	~SGMonster() override;

	static SGMonster* create(SGMonsterInfo* baseInfo, SGMapLayer* mapLayer, SGAIInfo* aiInfo);
	void initActorSprite() override;
	void initAIActivities() override;
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

	SGMonsterInfo* getBaseInfo();
private:
	SGMonsterInfo* m_pBaseInfo;
};


