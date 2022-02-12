/*
	�ۼ��� : ������
	���� ������
*/

#include <JCore/Core.h>
#include <JCore/Random.h>
#include <JCore/Exception.h>


namespace JCore {

std::default_random_engine Random::ms_DefaultRandomEngine;
bool Random::ms_bInitialized = false;


Random::Random() {
	// ó�� �ѹ��� �ʱ�ȭ�� ����.
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
		throw InvalidArgumentException("begin > end �Ǹ� �ȴ�");
	}

	std::uniform_int_distribution<> dist(begin, end - 1);
	return dist(ms_DefaultRandomEngine);
}

double Random::GenerateDouble(double begin, double end) {
	if (begin > end) {
		throw InvalidArgumentException("begin > end �Ǹ� �ȴ�");
	}

	std::uniform_real_distribution<> dist(begin, end);
	return dist(ms_DefaultRandomEngine);
}


} // namespace JCore