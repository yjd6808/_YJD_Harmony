#include <header.h>

struct A
{
	int Data1;
	int Data2;
	Vector<String> Data3;
};



int main() {
	
	{
		
		A Chlild;
		Chlild.Data1 = 0;
		Chlild.Data2 = 1;
		Chlild.Data3.PushBack("abcd");

		Vector<A> Parent;
		Parent.PushBack(Chlild);
		int c = 40;
	}


	SafeConsole::WriteLine("=================================================");
	return 0;
}

