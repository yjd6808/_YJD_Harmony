/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGCharacter.h>

class SGPlayer
{
public:
	SGPlayer();
	~SGPlayer();

	void setCharacter(SGCharacter* character) { m_pCharacter = character; }
	SGCharacter* getCharacter() { return m_pCharacter; }
private:
	SGCharacter* m_pCharacter;

};


