
#include "header.h"


using Console = NormalConsole;



int main() {

    Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);
	Thread thread;
	//std::thread th([] {Sleep(100);});
	thread.Start([](void*) {
		for (int i = 0; i < 1'100'000'000; i++) {
			std::cout << i << "\n";
		}
	});
	return 0;
}

