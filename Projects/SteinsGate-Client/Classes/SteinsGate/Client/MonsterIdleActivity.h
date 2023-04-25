﻿/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/MonsterActivity.h>

class MonsterIdleActivity : public MonsterActivity
{
public:
	MonsterIdleActivity(Monster* monster);

	void onUpdate(float dt) override;
	void onActivityBegin() override;
	void onActivityEnd() override;
};



