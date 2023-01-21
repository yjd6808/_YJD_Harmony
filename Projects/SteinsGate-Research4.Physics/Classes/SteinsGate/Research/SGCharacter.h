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
	SGCharacter(int code, const SGCharacterInfo& info);

	static SGCharacter* create(int code, const SGCharacterInfo& info);

	void initActorSprite() override;

private:


private:
	SGCharacterBaseInfo* m_pBaseInfo;
	SGCharacterInfo m_CharacterInfo;
};


