#include "header.h"
#include <stacktrace>



USING_NS_JC;
String s;


int main() {
	Console::SetSize(600, 800);

	String c = "c";       // c: "c"
	String d = "d";       // d: "d'
	String g = CopyConcat<String>("a", "b", c, d, "e"); // abcde
	

	return 0;
}

