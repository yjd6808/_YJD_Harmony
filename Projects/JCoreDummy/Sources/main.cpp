#include <header.h>


SpinLock l;


int main() {

	AutoMemoryLeakDetector detector;
	{
		
		HashMap<int, String> aq;
		aq.Insert(1, "fsefesfesfesf");
		aq.Insert(2, "fsefesfesfesf");
		aq.Remove(1);
	}
	int g2 = detector.StopDetect();

	SafeConsole::WriteLine("=================================================");
	return 0;
}

