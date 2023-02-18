/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:43 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGCharacter.h>

class SGMapLayer;
class SGPlayer : public SGCharacter
{
public:
	SGPlayer();
	~SGPlayer() override;

protected:
	PlayerData m_PlayerData;
};