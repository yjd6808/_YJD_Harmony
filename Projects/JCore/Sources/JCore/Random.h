/*
	작성자 : 윤정도
	난수 생성기
*/

#pragma once

#include <JCore/Exception.h>

#include <random>

NS_JC_BEGIN

struct Random final
{
	Random();

	template <typename T>
	T Generate(T inclusiveBegin, T inclusiveEnd) {
		if (inclusiveBegin > inclusiveEnd) {
			throw InvalidArgumentException("begin > end 되면 안댐");
		}

		std::uniform_int_distribution<T> dist(inclusiveBegin, inclusiveEnd);
		return dist(ms_DefaultRandomEngine);
	}

	static int GenerateInt(int inclusiveBegin, int exclusiveEnd);
	static double GenerateDouble(double inclusiveBegin, double inclusiveEnd);

	static void EngineInitialize();
private:
	inline static bool ms_bInitialized;
    inline static std::mt19937 ms_DefaultRandomEngine;
    inline static std::random_device ms_RandomDevice;
};

NS_JC_END