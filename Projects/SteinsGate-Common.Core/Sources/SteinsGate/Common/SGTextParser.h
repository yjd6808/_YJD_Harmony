/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:16 AM
 * =====================
 *
 */


#pragma once


#include "Core.h"

class SGTextParser
{
public:
	static char* parserFrameInfo(SGString& frameString,
		Out_ int& frameIndex,
		Out_ int& delay,
		Out_ int& frameEvent,
		Out_ int& frameEventCode);

	static void parserIntNumbers(const SGString& numStr, Out_ int* numArr, int count);
	static void parserFloatNumbers(const SGString& string, Out_ float* numArr, int count);
	static void parserFloatNumbers(char* source, int len, Out_ float* numArr, int count);
};



