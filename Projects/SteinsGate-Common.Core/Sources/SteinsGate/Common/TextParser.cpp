/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:23 AM
 * =====================
 *
 */

#include "Core.h"
#include "TextParser.h"

//////////////////////////////////////////////////////////////////////////////////////////
char* TextParser::ParseFrameInfo(const char* _pFrameString, int _len,
                                 JCORE_OUT int& _frameIndex,
                                 JCORE_OUT int& _delay,
                                 JCORE_OUT int& _frameEventCode)
{
	char* pSource = (char*)_pFrameString;
	int* pFrameValues[3];
	pFrameValues[0] = &_frameIndex;
	pFrameValues[1] = &_delay;
	pFrameValues[2] = &_frameEventCode;

	int ptrIndex = 0;
	int buffIndex = 0;
	char buff[16];

	for (int i = 0; i < _len + 1; ++i)
	{
		buff[buffIndex] = pSource[i];

		if (pSource[i] == ' ' || pSource[i] == NULL)
		{
			buff[buffIndex] = NULL;
			*pFrameValues[ptrIndex++] = std::atoi(buff);
			buffIndex = 0;

			// 프레임 이벤트, 프레임 이벤트 ID까지 읽은 경우
			// 인스턴트 공격 박스 일수도 있으니 이어서 파싱을 진행해야함
			if (ptrIndex == 4)
				return pSource + i + 1;

			continue;
		}

		++buffIndex;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextParser::ParseIntNumbers(const SGString& _numStr, JCORE_OUT int* _pNumArr, int _count)
{
	char* pSource = _numStr.Source();
	int ptrIndex = 0;
	int buffIndex = 0;
	int readCount = 0;
	char buff[16];

	for (int i = 0; i < _numStr.Length() + 1; ++i)
	{
		buff[buffIndex] = pSource[i];

		if (pSource[i] == ' ' || pSource[i] == NULL)
		{
			buff[buffIndex] = NULL;
			_pNumArr[ptrIndex++] = std::atoi(buff);
			buffIndex = 0;
			++readCount;
			continue;
		}

		++buffIndex;
	}

	DebugAssertMsg(readCount == _count, "읽은 숫자와 작성된 숫자가 틀립니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextParser::ParseFloatNumbers(const SGString& _numStr, JCORE_OUT float* _pNumArr, int _count)
{
	ParseFloatNumbers(_numStr.Source(), _numStr.Length(), _pNumArr, _count);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TextParser::ParseFloatNumbers(char* _pSource, int _len, JCORE_OUT float* _pNumArr, int _count)
{
	int ptrIndex = 0;
	int buffIndex = 0;
	int readCount = 0;
	char buff[64];

	for (int i = 0; i < _len + 1; ++i)
	{
		buff[buffIndex] = _pSource[i];

		if (_pSource[i] == ' ' || _pSource[i] == NULL)
		{
			buff[buffIndex] = NULL;
			_pNumArr[ptrIndex++] = (float)std::atof(buff);
			buffIndex = 0;
			++readCount;
			continue;
		}

		++buffIndex;
	}

	DebugAssertMsg(readCount == _count, "읽은 숫자와 작성된 숫자가 틀립니다.");
}
