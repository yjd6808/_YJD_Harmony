/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIGroup.h>

class UILogin : public SGUIGroup
{
public:
	static UILogin* createRetain();
	bool init() override;
protected:
};


