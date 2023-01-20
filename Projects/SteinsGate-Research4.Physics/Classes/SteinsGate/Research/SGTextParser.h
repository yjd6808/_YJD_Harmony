/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:16 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>

class SGTextParser
{
public:
	static void parserFrameInfo(const SGString& frameString,
		Out_ int& frameIndex,
		Out_ int& delay,
		Out_ int& frameEvent,
		Out_ int& frameEventCode);
};


