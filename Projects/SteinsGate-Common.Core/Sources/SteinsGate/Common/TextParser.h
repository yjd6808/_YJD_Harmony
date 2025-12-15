/*
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
	static char* ParseFrameInfo(const char* _pFrameString, int _len,
	                            JCORE_OUT int& _frameIndex,
	                            JCORE_OUT int& _delay,
	                            JCORE_OUT int& _frameEventCode);

	static void ParseIntNumbers(const SGString& _numStr, JCORE_OUT int* _pNumArr, int _count);
	static void ParseFloatNumbers(const SGString& _numStr, JCORE_OUT float* _pNumArr, int _count);
	static void ParseFloatNumbers(char* _pSource, int _len, JCORE_OUT float* _pNumArr, int _count);
};
