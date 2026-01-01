/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 4:54:02 AM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Digit.h>

NS_JC_BEGIN

const char* Digit::HexChar = "0123456789ABCDEF";

int Digit::Length(int _num)
{
	int len = 1;
	while ((_num /= 10) > 0)
	{
		len++;
	}
	return len;
}

NS_JC_END
