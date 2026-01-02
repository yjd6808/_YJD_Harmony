/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:17 AM
 * =====================
 *
 */


#pragma once


#include <sgcl/AIActivity.h>

class SitActivity : public AIActivity
{
public:
	SitActivity(Actor* _pActor);
	void OnUpdate(float _dt) override;
};
