/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 9:15:13 AM
 * =====================
 *
 */




#include <SteinsGate/Common/Engine.h>

#include "SGRandomEx.h"

bool SGRandomEx::check(float prob) {
	if (SGRandom::random_real(0.0f, 100.0f) <= prob) {
		return true;
	}

	return false;
}