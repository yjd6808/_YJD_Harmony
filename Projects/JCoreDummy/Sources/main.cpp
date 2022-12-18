#include <header.h>

int main() {
	JCore::Initialize();
	ThreadLocal<int> g1;
	g1.Get();
	ThreadLocal<int> g2;
	g2.Get();
	g2.Get();
	g2.Get();
	g2.Get();
	ThreadLocal<int> g3;
	g3.Get();

	auto g22 = []() {
		static ThreadLocal<double> esfsefse1;
		static ThreadLocal<double> esfsefse2;
		static ThreadLocal<double> esfsefse3;
		static ThreadLocal<double> esfsefse4;
	};
	g22();

	
	Thread th([](void*) {
		ThreadLocal<int> g4;
		ThreadLocal<int> g5;
		g4.Get();
		g4.Get();
		g4.Get();
		g4.Get();
		g5.Get();
		g5.Get();
		g5.Get();
	});
	th.AutoJoin(true);

	

	Thread::Sleep(1000);
	JCore::Finalize();
	SafeConsole::WriteLine("=================================================");
	return 0;
}

