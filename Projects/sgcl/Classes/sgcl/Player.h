 
/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:43 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Character.h>

class MapLayer;

class Player : public Character
{
public:
	Player();
	~Player() override;

	void Initialize() override;
	void InitPlayerData();
};
