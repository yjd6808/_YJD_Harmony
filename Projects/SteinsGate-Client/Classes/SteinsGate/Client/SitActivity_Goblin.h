/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:17 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SitActivity.h>

class SitActivity_Goblin : public SitActivity
{
public:
	SitActivity_Goblin(Actor* actor);
	void onActivityBegin() override;
};


