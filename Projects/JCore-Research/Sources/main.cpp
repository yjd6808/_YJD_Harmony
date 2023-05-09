#include <bitset>

#include "header.h"
#include <stacktrace>

#include "JCore/Bit.h"


USING_NS_JC;
String s;

template <typename T, typename... Args>
void fn2(T val, Args... args) {
	int sum = 0;
	std::initializer_list<int> li{ (sum += args)... };
	printf("%d\n", sum);
}

template <typename... Args>
void fn(Args... args) {
	fn2((args += 30)...);
}

int main() {
	
	fn((short)0, (int)1, (long long)2, (char)3);


	Console::SetSize(600, 800);

	return 0;
}
