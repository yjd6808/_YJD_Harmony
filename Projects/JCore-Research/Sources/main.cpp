#include <bitset>

#include "header.h"
#include <stacktrace>

#include "JCore/Bit.h"


USING_NS_JC;
String s;


int main() {
	

	Console::SetSize(600, 800);
	HashMap<String, int*> s;
	for (int i = 0; i < 27; ++i) {
		String gg = " fsd";
		s.Insert(gg, nullptr);
	}
	return 0;
}
