/*
	작성자 : 윤정도
	난수 생성기
*/

#include <jc/Core.h>
#include <jc/Random.h>
#include <jc/Exception.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
Random::Random()
{
	// 처음 한번만 초기화를 하자.
	if (!Initialized)
	{
		Initialized = true;
		DefaultRandomEngine.seed(RandomDevice());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Random::EngineInitialize()
{
	DefaultRandomEngine.seed(RandomDevice());
}

//////////////////////////////////////////////////////////////////////////////////////////
void Random::WriteRandomAlphabatTextBuffered(int _length, char* _pBuff, int _capacity)
{
	int index = 0;

	while (index < _capacity && index < _length)
	{
		int alphabetIndex = GenerateInt(0, 51);

		if (alphabetIndex >= 26)
		{
			alphabetIndex -= 26;
			_pBuff[index] = 'a' + static_cast<char>(alphabetIndex);
		}
		else
		{
			_pBuff[index] = 'A' + static_cast<char>(alphabetIndex);
		}

		++index;
	}

	_pBuff[index] = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////
int Random::GenerateInt(int _inclusiveBegin, int _exclusiveEnd)
{
	if (_inclusiveBegin > _exclusiveEnd)
	{
		throw InvalidArgumentException("begin > end 되면 안댐");
	}

	std::uniform_int_distribution<> dist(_inclusiveBegin, _exclusiveEnd - 1);
	return dist(DefaultRandomEngine);
}

//////////////////////////////////////////////////////////////////////////////////////////
double Random::GenerateDouble(double _inclusiveBegin, double _inclusiveEnd)
{
	if (_inclusiveBegin > _inclusiveEnd)
	{
		throw InvalidArgumentException("begin > end 되면 안댐");
	}

	std::uniform_real_distribution<> dist(_inclusiveBegin, _inclusiveEnd);
	return dist(DefaultRandomEngine);
}

//////////////////////////////////////////////////////////////////////////////////////////
char Random::GenerateAlphabat()
{
	static char Alphabats[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return Alphabats[GenerateInt(0, static_cast<int>(sizeof Alphabats))];
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Random::Chance(float _percentProbability)
{
	return GenerateF(0.0f, 100.0f) <= _percentProbability;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Random::Chance(double _percentProbability)
{
	return GenerateF(0.0, 100.0) <= _percentProbability;
}

NS_END
