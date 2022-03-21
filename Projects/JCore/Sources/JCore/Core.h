/*
	�ۼ��� : ������
	�ۼ��� : 2021-01-03(��)

	PCH �����Դϴ�.
*/

#pragma once

#pragma warning (disable : 4267)		// conversion from 'size_t' to 'type', possible loss of data
#pragma warning (disable : 26439)		// This kind of function may not throw.Declare it 'noexcept' (f.6)

#define _CRT_SECURE_NO_WARNINGS

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


#ifndef _In_
	#define _In_
#endif

#ifndef _Out_
	#define _Out_	
#endif

#ifndef _InOut_
	#define _InOut_	
#endif