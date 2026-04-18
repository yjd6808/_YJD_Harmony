/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:16 AM
 * =====================
 *
 */


#pragma once

class TextUtil
{
public:
	static char* ParseFrameInfo(const char* _pFrameString, int _len,
	                            OUT int& _frameIndex,
	                            OUT int& _delay,
	                            OUT int& _frameEventCode);

	static void ParseIntNumbers(const jc::String& _numStr, OUT int* _pNumArr, int _count);
	static void ParseFloatNumbers(const jc::String& _numStr, OUT float* _pNumArr, int _count);
	static void ParseFloatNumbers(char* _pSource, int _len, OUT float* _pNumArr, int _count);
};
