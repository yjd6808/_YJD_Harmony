/*
	작성자 : 윤정도
	작성일 : 2021-01-03(수)

	PCH 파일입니다.
*/

#pragma once

#pragma warning (disable : 4267)		// conversion from 'size_t' to 'type', possible loss of data
#pragma warning (disable : 26439)		// This kind of function may not throw.Declare it 'noexcept' (f.6)
#pragma warning (disable : 4018)		// '<': signed / unsigned mismatch

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN	

#include <iostream>
#include <thread>
#include <Windows.h>
#include <exception>
#include <random>
#include <assert.h>

#define DebugAssert(exp, msg)		assert((exp) && msg)

#define DeleteSafe(x)			\
do {							\
	if (x) {					\
		delete x;				\
	}							\
	x = nullptr;				\
} while (0);					

#define DeleteArraySafe(x)		\
do {							\
	if (x) {					\
		delete[] x;				\
	}							\
	x = nullptr;				\
} while (0);					


#define DeletePlacementSafe(x, Type)	\
do {									\
	if (x) {							\
		x->~Type();						\
	}									\
} while (0);	


#ifndef In_
	#define In_
#endif

#ifndef Out_
	#define Out_
#endif

#ifndef InOut_
	#define InOut_
#endif

#ifndef InOpt_
	#define InOpt_
#endif
