/*
	작성자 : 윤정도
	PCH 파일입니다.
*/


#pragma once

#pragma warning(disable 26495) //  is uninitialized.Always initialize a member variable(type.6)
#pragma warning(disable 26812) //  is unscoped.Prefer 'enum class' over 'enum' (Enum.3)

#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <utility>
#include <string_view>
#include <type_traits>

#define ON		1
#define OFF		0

#define	TEST_ArraysTest					ON
#define	TEST_AVLTreeImplTest			ON
#define	TEST_AVLTreeMemoImplTest		ON
#define TEST_MemoryUtilTest				ON
#define TEST_BinarySearchTreeImplTest	ON
#define TEST_TwoThreeFourTreeImplTest	OFF
#define TEST_StringTest					ON
#define TEST_StringUtilTest				ON
#define TEST_CoreTest					ON
#define TEST_MathTest					ON
#define TEST_RandomTest					OFF
#define TEST_TypeTraitsTest				ON


template <typename... Args>
void Print(const char* fmt, Args&&... args) {
	printf(fmt, JCore::Forward<Args>(args)...);
}

template <typename... Args>
void PrintLine(const char* fmt, Args&&... args) {
	printf(fmt, JCore::Forward<Args>(args)...);
	printf("\n");
}

template <typename... Args>
void PrintLine(Args&&... args) {
	if constexpr (sizeof...(args) == 0) {
		printf("\n");
	}
}