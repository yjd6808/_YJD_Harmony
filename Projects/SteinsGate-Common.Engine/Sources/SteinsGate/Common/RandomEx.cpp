/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 9:15:13 AM
 * =====================
 *
 */




#include "Engine.h"

#include "RandomEx.h"

bool RandomEx::check(float prob) {
	if (SGRandom::random_real(0.0f, 100.0f) <= prob) {
		return true;
	}

	return false;
}