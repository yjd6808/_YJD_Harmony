﻿/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:43 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Character.h>

class MapLayer;
class Player : public Character
{
public:
	Player();
	~Player() override;

protected:
	PlayerData m_PlayerData;
};