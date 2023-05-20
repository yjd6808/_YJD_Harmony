﻿/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:16 AM
 * =====================
 *
 */


#pragma once


#include "Core.h"

class TextParser
{
public:
	static char* parserFrameInfo(SGString& frameString,
		JCORE_OUT int& frameIndex,
		JCORE_OUT int& delay,
		JCORE_OUT int& frameEvent,
		JCORE_OUT int& frameEventCode);

	static void parserIntNumbers(const SGString& numStr, JCORE_OUT int* numArr, int count);
	static void parserFloatNumbers(const SGString& string, JCORE_OUT float* numArr, int count);
	static void parserFloatNumbers(char* source, int len, JCORE_OUT float* numArr, int count);
};



