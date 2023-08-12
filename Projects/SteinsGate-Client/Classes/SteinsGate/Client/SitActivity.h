/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:17 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/AIActivity.h>

class SitActivity : public AIActivity
{
public:
	SitActivity(Actor* actor);
	void onUpdate(float dt) override;
};


