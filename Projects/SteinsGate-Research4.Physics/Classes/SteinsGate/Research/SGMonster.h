/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:38 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGMonsterInfo.h>

class SGMonster : public SGActor
{
public:
	SGMonster(SGMonsterInfo* baseInfo);
	static SGMonster* create(SGMonsterInfo* baseInfo);
	void initActorSprite() override;

	void update(float dt) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	SGMonsterInfo* getBaseInfo();
private:
	SGMonsterInfo* m_pBaseInfo;
};


