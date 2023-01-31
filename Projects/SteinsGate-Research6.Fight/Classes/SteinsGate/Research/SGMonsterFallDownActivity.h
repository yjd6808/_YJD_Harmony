/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGAITimedActivity.h>

class SGMonsterFallDownActivity : public SGAIActivity
{
public:
	SGMonsterFallDownActivity(SGAIActor* actor);

	void onActivityBegin() override;
	void onUpdate(float dt) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
};




