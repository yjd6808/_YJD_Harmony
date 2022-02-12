/*
	작성자 : 윤정도
	난수 생성기
*/

#include <JCore/Core.h>
#include <JCore/Random.h>
#include <JCore/Exception.h>


namespace JCore {

std::default_random_engine Random::ms_DefaultRandomEngine;
bool Random::ms_bInitialized = false;


Random::Random() {
	// 처음 한번만 초기화를 하자.
	if (!ms_bInitialized) {
		ms_bInitialized = true;
		ms_DefaultRandomEngine.seed(time(0));
	}
}


/*  =======================================================
					   Public
    ======================================================= */

void Random::EngineInitialize() {
	ms_DefaultRandomEngine.seed(time(0));
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


} // namespace JCore