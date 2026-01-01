/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <sgcl/FallDownActivity.h>

class FallDownActivity_Goblin : public FallDownActivity
{
public:
	FallDownActivity_Goblin(Actor* _pActor);

	void OnActivityBegin() override;
	void OnActivityEnd() override;
	void OnUpdate(float _dt) override;

private:
	bool bounced_;
	bool down_;
	float elapsedDownTime_;
	float downRecoverTime_;
};
