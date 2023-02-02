/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGCharProjectileListener.h>

class SGPlayer;
class SGGunnerBulletListener : public SGCharProjectileListener
{
public:
	void onCreated() override;

	SGActorListener* createNew() override { return new SGGunnerBulletListener; }
};


