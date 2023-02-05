#include <header.h>



struct A
{
	int Data1;
	int Data2;
	Vector<String> Data3;
};
union fse
{
	std::string s;
	fse() {}
	~fse() {}
};



int main() {
	DebugAssertFmt(false, "11");
	SafeConsole::WriteLine("=================================================");
	return 0;
}

