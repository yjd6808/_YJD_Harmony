/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 6:23:23 AM
 * =====================
 *
 */


#include "SGTextParser.h"

void SGTextParser::parserFrameInfo(const SGString& frameString, 
	Out_ int& frameIndex, 
	Out_ int& delay,
	Out_ int& frameEvent,
	Out_ int& frameEventCode) {

	char* pSoruce = frameString.Source();
	int* p[4];
	p[0] = &frameIndex;
	p[1] = &delay;
	p[2] = &frameEvent;
	p[3] = &frameEventCode;

	int iPtrIdx = 0;
	int iBuffIdx = 0;
	char buff[16];

	for (int i = 0; i < frameString.Length() + 1; ++i) {
		buff[iBuffIdx] = pSoruce[i];

		if (pSoruce[i] == ' ' || pSoruce[i] == NULL) {
			buff[iBuffIdx] = NULL;
			*p[iPtrIdx++] = std::atoi(buff);
			iBuffIdx = 0;
			continue;
		}

		++iBuffIdx;
	}
}
