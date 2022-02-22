/*
	�ۼ��� : ������
	���� ������
*/

#pragma once

#include <JCore/Exception.h>

#include <random>

namespace JCore {

struct Random final
{
	Random();

	template <typename T>
	T Generate(T inclusiveBegin, T inclusiveEnd) {
		if (inclusiveBegin > inclusiveEnd) {
			throw InvalidArgumentException("begin > end �Ǹ� �ȴ�");
		}

		std::uniform_int_distribution<T> dist(inclusiveBegin, inclusiveEnd);
		return dist(ms_DefaultRandomEngine);
	}

	int GenerateInt(int inclusiveBegin, int exclusiveEnd);
	double GenerateDouble(double inclusiveBegin, double inclusiveEnd);

	static void EngineInitialize();
private:
	static bool ms_bInitialized;
	static std::default_random_engine ms_DefaultRandomEngine;
};

} // namespace JCore