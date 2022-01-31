#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/Container/Arrays.h>

using namespace JCore;
using namespace std;

int main() {
	/*
	IsIntegerType_v<const volatile int&>;

	char a[30];
	IsStringType_v<String>;
	IsStringType_v<char*>;
	IsStringType_v<const char*>;
	IsStringType_v<decltype(a)>;

	IsFloatingPointType_v<double>;
	IsFloatingPointType_v<float>;
	*/


	Time::DateTime now = Time::DateTime::Now();
	auto cc = now.GetDayOfWeek();
	try {
		cout << now.Format("MM/dd/yyyy") << "\n";
		cout << now.Format("dddd, dd MMMM yyyy") << "\n";
		cout << now.Format("dddd, dd MMMM yyyy HH:mm:ss") << "\n";
		cout << now.Format("MM/dd/yyyy HH:mm")	 << "\n";
		cout << now.Format("MM/dd/yyyy hh:mm tt") << "\n";
		cout << now.Format("MM/dd/yyyy H:mm") << "\n";
		cout << now.Format("MM/dd/yyyy h:mm tt") << "\n";
		cout << now.Format("MM/dd/yyyy HH:mm:ss") << "\n";
		cout << now.Format("MMMM dd") << "\n";
		cout << now.Format("yyyy-MM-ddTHH:mm:ss.ffffffK") << "\n";
		cout << now.Format("ddd, dd MMM yyy HH:mm:ss GMT") << "\n";
		cout << now.Format("yyyy-MM-ddTHH:mm:ss") << "\n";
		cout << now.Format("HH:mm") << "\n";
		cout << now.Format("hh:mm tt") << "\n";
		cout << now.Format("H:mm") << "\n";
		cout << now.Format("h:mm tt")	 << "\n";
		cout << now.Format("HH:mm:ss") << "\n";
		cout << now.Format("yyyy MMMM") << "\n";
	} catch (std::exception& ex) {
		cout << ex.what() << "\n";
	}
	
	Time::DateAndTime now2 = now.ToDateAndTime();
	//now2.AddMicroSecond(Time::Detail::TicksPerHour * 10 + Time::Detail::TicksPerMinute * 125);
	now2.AddDay(5);
	now2.AddMonth(3);

	return 0;
}

