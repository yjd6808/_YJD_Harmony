#pragma once

#include <random>

/*
	작성자 : 윤정도
	난수 생성기
*/

namespace JCore {

struct Random final
{
	Random();

	int GenerateInt(int begin, int end);
	double GenerateDouble(double begin, double end);
private:
	static bool ms_bInitialized;
	static std::default_random_engine ms_DefaultRandomEngine;
};

} // namespace JCore