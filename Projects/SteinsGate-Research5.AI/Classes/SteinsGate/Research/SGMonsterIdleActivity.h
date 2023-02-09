/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGAITimedActivity.h>

class SGMonsterIdleActivity : public SGAITimedActivity
{
public:
	SGMonsterIdleActivity(SGAIActor* actor);

	void onActivityBegin() override;
	void onActivityEnd() override;
};




