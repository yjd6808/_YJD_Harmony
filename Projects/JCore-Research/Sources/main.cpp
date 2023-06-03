#include "header.h"

using namespace std::chrono_literals;

USING_NS_STD_CHRONO;

//system_clock::time_point ToTimePoint() {
//	auto t = DateTime::Now().GetTick();
//	t -= Detail::ADBeginTick_v;
//	const Int32 uiBias = DateTime::TimeZoneBiasMinute();
//	t -= (uiBias * -1) * Detail::TicksPerMinute_v;
//	return system_clock::time_point(system_clock::duration{t});
//}

//auto a = system_clock::now();
//duration<double> kk = a.time_since_epoch();
//auto b = duration_cast<seconds>(kk);
//auto k = ToTimePoint();

int main() {
	TreeMap<int, int> tm;
	Random::EngineInitialize();
	for (int i = 0; i < 100; ++i) {
		tm.Insert(i*10, i*10);
	}

	for (int i = -1; i < 1100; ++i) {
		int* pv = tm.UpperBoundValue(i);
		if (pv == nullptr)
			Console::WriteLine("[%d] %s", i, "없음");
		else
			Console::WriteLine("[%d] %d", i, *pv);
	}
	

	
	
	return 0;
}







