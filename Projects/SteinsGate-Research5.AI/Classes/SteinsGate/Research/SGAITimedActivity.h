/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:20 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/SGAIActivity.h>


class SGAITimedActivity : public SGAIActivity {
public:
	SGAITimedActivity(SGAIActor* actor, AIActivity_t type);
	~SGAITimedActivity() override = default;

	void setLimit(float limit);
	void onUpdate(float dt) override;
	void onActivityBegin();
protected:
	float m_fElasedTime;
	float m_fLimitTime;
};


