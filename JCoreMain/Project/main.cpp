#include <iostream>

#include <JCore/Core.h>
#include <JCore/String/StringUtil.h>
#include <JCore/String/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>

#include <random>

using namespace JCore;
using namespace std;


template <typename T>
static constexpr T Max(const T lhs, const T rhs) {
	return lhs > rhs ? lhs : rhs;
}

template <typename T, typename... Args>
static constexpr T Max(const T arg0, const Args... args) {
	return Max(arg0, Max(args...));
}


int main() {
	cout << Max(1, 2, 6, 4) << "\n";

	return 0;
}


