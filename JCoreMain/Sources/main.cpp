#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/Container/Arrays.h>

using namespace JCore;
using namespace std;

// 1643459478796342700
int main() {
	/*
	IsIntegerType_v<const volatile int&>;

	char a[30];
	IsStringType_v<String>;
	IsStringType_v<char*>;
	IsStringType_v<const char*>;
	IsStringType_v<decltype(a)>;

	IsFloatingPointType_v<double>;
	IsFloatingPointType_v<float>;
	*/


	DateTime now = DateTime::Now();
	DateAndTime dateAndTime = now.ToDateAndTime();

	return 0;
}

