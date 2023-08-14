#include "header.h"

#define EXPECT_TRUE(exp) DebugAssert(exp)

struct Model
{
	Model();
	Model(int _) : a(_) {}
	Model(int _1, int _2) : a(_1), b(_2) {}

	int a = 3;
	int b = 3;
};


int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	//{


	//	// PushBackAll 테스트
	//	{
	//		JCORE_LEAK_CHECK_ASSERT;
	//		Vector v{ new int{0} };

	//		v.PushBack(new int{ 1 });
	//		v.PushBack(new int{ 2 });
	//		v.PushBack(new int{ 3 });

	//	}

	//}
	return 0;
}

