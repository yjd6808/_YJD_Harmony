#include "header.h"

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	{
		TreeSet<int> s;
		for (int i = 0; i < 100; ++i) {
			int v = Random::GenerateInt(0, 100);
			bool x = s.Insert(v);
			if (x == false) {
				Console::WriteLine("%d가 이미 존재함", v);
			}
		}
		s.ForEach([](int v) { Console::WriteLine("%d", v); });
	}

	{
		Console::WriteLine("=========================================");
		Console::WriteLine("=========================================");
		Console::WriteLine("=========================================");
		HashSet<int> s;
		for (int i = 0; i < 100; ++i) {
			int v = Random::GenerateInt(0, 100);
			bool x = s.Insert(v);
			if (x == false) {
				Console::WriteLine("%d가 이미 존재함", v);
			}
		}
		s.ForEach([](int v) { Console::WriteLine("%d", v); });
	}


	return 0;
}

