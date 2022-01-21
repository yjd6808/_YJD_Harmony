#include <iostream>

#include <JCore/Core.h>
#include <JCore/String/StringUtil.h>
#include <JCore/String/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>


#include <random>
using namespace JCore;
using namespace std;
#include <utility>



#include <initializer_list>

// we cannot return a char array from a function, therefore we need a wrapper
template <unsigned N>
struct Str
{
	char c[N];
};


template <typename T, Int32U Size>
struct CTBuffer
{
	T _[Size];

	constexpr T operator[](const int idx) const {
		return _[idx];
	}
};



// 컴파일 타임 문자열
template <Int32U Size>
struct CTString
{
	// 문자열 합치기
	constexpr auto Remove(const char t) const {
		CTString<Size>str{};
		CTBuffer<int, Size> buff{};

		str.Str[Size - 1] = NULL;
		char* ptr = str.Str;
		int idx = 0;

		for (int i = 0; i < Size; i++) {
			if (Str[i] == t) {
				buff._[i] = i;
			}
		}

		return buff;
	}

	char Str[Size];
};





int main() {
	constexpr CTString<5> a{ "abcd" };
	constexpr auto b = a.Remove('c');


	return 0;
}


