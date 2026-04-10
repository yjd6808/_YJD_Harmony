/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Random API를 호출하기 위한 extern "C" 래퍼 구현
 */

#include "Extern_Random.h"
#include "jc/Random.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void RandomInit()
{
	globalRandomInstance.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
int RandomGenerateInt(int _inclusiveBegin, int _inclusiveEnd)
{
	return globalRandomInstance.GenerateInt(_inclusiveBegin, _inclusiveEnd);
}

//////////////////////////////////////////////////////////////////////////////////////////
double RandomGenerateDouble(double _inclusiveBegin, double _inclusiveEnd)
{
	return globalRandomInstance.GenerateDouble(_inclusiveBegin, _inclusiveEnd);
}

//////////////////////////////////////////////////////////////////////////////////////////
char RandomGenerateAlphabat()
{
	return globalRandomInstance.GenerateAlphabat();
}

//////////////////////////////////////////////////////////////////////////////////////////
int RandomChance(double _percentProbability)
{
	return globalRandomInstance.Chance(_percentProbability) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RandomWriteAlphabatTextBuffered(int _length, char* _pBuff, int _capacity)
{
	if (_pBuff == nullptr || _capacity <= 0)
		return;

	globalRandomInstance.WriteRandomAlphabatTextBuffered(_length, _pBuff, _capacity);
}
