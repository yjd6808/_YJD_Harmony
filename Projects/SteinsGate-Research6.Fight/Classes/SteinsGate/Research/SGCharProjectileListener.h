/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 5:52:13 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGProjectileListener.h>

class SGPlayer;
class SGCharacter;
class SGCharProjectileListener : public SGProjectileListener
{
public:
	SGCharProjectileListener();

	void initPlayer();
	bool isPlayerProjectile();
protected:
	SGPlayer* m_pPlayer;
	SGCharacter* m_pCharacter;
	bool m_bPlayerProjectile;
};


