/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 4:54:02 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Digit.h>

NS_JC_BEGIN

int Digit::Length(int num) {
	int iLen = 1;
	while ((num /= 10) > 0) {
		iLen++;
	}
	return iLen;
}

NS_JC_END
