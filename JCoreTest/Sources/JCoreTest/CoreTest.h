/*
	작성자 : 윤정도
	PCH 파일입니다.
*/


#pragma once

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

#define TEST_MemoryUtilTest				ON
#define TEST_BinarySearchTreeImplTest	OFF
#define TEST_TwoThreeFourTreeImplTest	ON
#define TEST_StringTest					ON
#define TEST_StringUtilTest				ON
#define TEST_CoreTest					ON
#define TEST_RandomTest					OFF
#define TEST_TypeTraitsTest				ON