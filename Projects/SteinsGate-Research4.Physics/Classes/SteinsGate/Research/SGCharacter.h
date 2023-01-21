/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGConfigManager.h>

class SGCharacter : public SGActor
{
public:
	SGCharacter(int code);

	static SGCharacter* create(int code);

private:
	SGCharacterBaseInfo* m_pBaseInfo;
};


