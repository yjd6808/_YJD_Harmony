#pragma once

#include <random>

/*
	�ۼ��� : ������
	���� ������
*/

namespace JCore {

struct Random final
{
	Random();

	template <typename T>
	T Generate(T begin, T end) {
		if (begin > end) {
			throw std::invalid_argument("begin > end �Ǹ� �ȴ�");
		}

		std::uniform_int_distribution<T> dist(begin, end - 1);
		return dist(ms_DefaultRandomEngine);
	}

	int GenerateInt(int begin, int end);
	double GenerateDouble(double begin, double end);
private:
	static bool ms_bInitialized;
	static std::default_random_engine ms_DefaultRandomEngine;
};

} // namespace JCore