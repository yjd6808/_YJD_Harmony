/*
	작성자 : 윤정도
	난수 생성기
*/

#include <JCore/Core.h>
#include <JCore/Random.h>
#include <JCore/Exception.h>


NS_JC_BEGIN

Random::Random() {
	// 처음 한번만 초기화를 하자.
	if (!ms_bInitialized) {
		ms_bInitialized = true;
		ms_DefaultRandomEngine.seed(ms_RandomDevice());
	}
}


/*  =======================================================
					   Public
    ======================================================= */

void Random::EngineInitialize() {
	ms_DefaultRandomEngine.seed(ms_RandomDevice());
}

void Random::WriteRandomAlphabatTextBuffered(int length, char* buff, int capacity) {
	int i = 0;
	while (i < capacity && i < length) {
		int idx = GenerateInt(0, 51);
		if (idx >= 26) {
			idx -= 26;
			buff[i] = 'a' + char(idx);
		} else {
			buff[i] = 'A' + char(idx);
		}
		++i;
	}
	buff[i] = NULL;
}

int Random::GenerateInt(int begin, int end) {
	if (begin > end) {
		throw InvalidArgumentException("begin > end 되면 안댐");
	}

	std::uniform_int_distribution<> dist(begin, end - 1);
	return dist(ms_DefaultRandomEngine);
}

double Random::GenerateDouble(double begin, double end) {
	if (begin > end) {
		throw InvalidArgumentException("begin > end 되면 안댐");
	}

	std::uniform_real_distribution<> dist(begin, end);
	return dist(ms_DefaultRandomEngine);
}

char Random::GenerateAlphabat() {
	static char s_Alphabats[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return s_Alphabats[GenerateInt(0, sizeof s_Alphabats)];
}

bool Random::Chance(float percentProbability) {
	return GenerateF(0.0f, 100.0f) <= percentProbability;
}

bool Random::Chance(double percentProbability) {
	return GenerateF(0.0, 100.0) <= percentProbability;
}


NS_JC_END
