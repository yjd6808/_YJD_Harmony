/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 12:57:33 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/ComboTree.h>

class PlayerActionManager
{
public:
	PlayerActionManager() {}

	void update(float dt);
private:


	ComboTree m_ComboTree;
};