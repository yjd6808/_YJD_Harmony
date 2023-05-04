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
	HashMap<int, int> a;


	for (int i = 0; i < 30; ++i) {
		a.Insert(i, i + 1);
		printf("\n\t페어 순회\n");
		a.ForEach([](Pair<int, int>& p) {
			printf("\t(%d, %d)\n", p.Key, p.Value);
		});
		printf("\n\t키 순회\n");
		a.ForEachKey([](int& k) {
			printf("\t(%d)\n", k);
		});
		printf("\n\t갑 순회\n");
		a.ForEachValue([](int& v) {
			printf("\t(%d)\n", v);
		});
		printf("===\n");
	}

	return 0;
}
