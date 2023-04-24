/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 5:52:13 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ProjectileListener.h>

class HostPlayer;
class Character;
class CharProjectileListener : public ProjectileListener
{
public:
	CharProjectileListener();

	void initPlayer();
	bool isPlayerProjectile();
protected:
	HostPlayer* m_pPlayer;
	Character* m_pCharacter;
	bool m_bPlayerProjectile;
};


