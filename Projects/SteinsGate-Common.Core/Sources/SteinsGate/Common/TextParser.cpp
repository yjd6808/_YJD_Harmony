/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:23 AM
 * =====================
 *
 */

#include "Core.h"
#include "TextParser.h"

char* TextParser::parseFrameInfo(const char* frameString, int len,
	JCORE_OUT int& frameIndex, 
	JCORE_OUT int& delay,
	JCORE_OUT int& frameEventCode) {

	char* pSource = (char*)frameString;
	int* p[3];
	p[0] = &frameIndex;
	p[1] = &delay;
	p[2] = &frameEventCode;

	int iPtrIdx = 0;
	int iBuffIdx = 0;
	char buff[16];

	for (int i = 0; i < len + 1; ++i) {
		buff[iBuffIdx] = pSource[i];

		if (pSource[i] == ' ' || pSource[i] == NULL) {
			buff[iBuffIdx] = NULL;
			*p[iPtrIdx++] = std::atoi(buff);
			iBuffIdx = 0;

			// 프레임 이벤트, 프레임 이벤트 ID까지 읽은 경우
			// 인스턴트 공격 박스 일수도 있으니 이어서 파싱을 진행해야함
			if (iPtrIdx == 4) {
				return pSource + i + 1;
			}
			continue;
		}

		++iBuffIdx;
	}

	return nullptr;
}

void TextParser::parseIntNumbers(const SGString& numStr, JCORE_OUT int* numArr, int count) {
	char* pSoruce = numStr.Source();
	int iPtrIdx = 0;
	int iBuffIdx = 0;
	int iReadCount = 0;
	char buff[16];

	for (int i = 0; i < numStr.Length() + 1; ++i) {
		buff[iBuffIdx] = pSoruce[i];

		if (pSoruce[i] == ' ' || pSoruce[i] == NULL) {
			buff[iBuffIdx] = NULL;
			numArr[iPtrIdx++] = std::atoi(buff);
			iBuffIdx = 0;
			++iReadCount;
			continue;
		}

		++iBuffIdx;
	}

	DebugAssertMsg(iReadCount == count, "읽은 숫자와 작성된 숫자가 틀립니다.");
}

void TextParser::parseFloatNumbers(const SGString& numStr, float* numArr, int count) {
	parseFloatNumbers(numStr.Source(), numStr.Length(), numArr, count);
	
}

void TextParser::parseFloatNumbers(char* source, int len, float* numArr, int count) {
	int iPtrIdx = 0;
	int iBuffIdx = 0;
	int iReadCount = 0;
	char buff[64];

	for (int i = 0; i < len + 1; ++i) {
		buff[iBuffIdx] = source[i];

		if (source[i] == ' ' || source[i] == NULL) {
			buff[iBuffIdx] = NULL;
			numArr[iPtrIdx++] = (float)std::atof(buff);
			iBuffIdx = 0;
			++iReadCount;
			continue;
		}

		++iBuffIdx;
	}

	DebugAssertMsg(iReadCount == count, "읽은 숫자와 작성된 숫자가 틀립니다.");
}
