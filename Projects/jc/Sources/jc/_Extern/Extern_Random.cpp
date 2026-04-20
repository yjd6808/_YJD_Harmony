/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Random API를 호출하기 위한 extern "C" 래퍼 구현
 */

#include "Extern_Random.h"
#include "jc/Random.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void Random_Init()
{
	globalRandomInstance.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Random_GenerateInt(_s32 _inclusiveBegin, _s32 _inclusiveEnd)
{
	return globalRandomInstance.GenerateInt(_inclusiveBegin, _inclusiveEnd + 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 Random_GenerateDouble(_f64 _inclusiveBegin, _f64 _inclusiveEnd)
{
	return globalRandomInstance.GenerateDouble(_inclusiveBegin, _inclusiveEnd);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 Random_GenerateAlphabat()
{
	return globalRandomInstance.GenerateAlphabat();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Random_Chance(_f64 _percentProbability)
{
	return globalRandomInstance.Chance(_percentProbability) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Random_WriteAlphabatTextBuffered(_s32 _length, _s8* _pBuff, _s32 _capacity)
{
	if (_pBuff == nullptr || _capacity <= 0)
		return;

	globalRandomInstance.WriteRandomAlphabatTextBuffered(_length, _pBuff, _capacity);
}
