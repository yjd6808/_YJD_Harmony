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

#define force_cast				reinterpret_cast

#define JSAFE_DELETE(x)			\
do {							\
	if (x) {					\
		delete x;				\
	}							\
	x = nullptr;				\
} while (0);					

#define JSAFE_DELETE_ARRAY(x)	\
do {							\
	if (x) {					\
		delete[] x;				\
	}							\
	x = nullptr;				\
} while (0);					

