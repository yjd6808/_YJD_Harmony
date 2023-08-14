#include <JCore/Container/TreeTable.h>
#include <JCore/Macro.h>
#include <JCore/Debug/MemoryLeakDetector.h>

#define EXPECT_TRUE(exp) DebugAssert(exp)

USING_NS_JC;

int main() {
	//Console::SetSize(800, 600);
	//Random::EngineInitialize();

	{
		JCore::AutoMemoryLeakDetector JCORE_CONCAT_COUNTER(_) { [](Int32U leakedBytes) { DebugAssertMsg(leakedBytes == 0, "%u 바이트 메모리 릭이 있습니다.", leakedBytes); } };
		TreeTable<ParameterPack_t<int*, Comparator<int*>, DefaultAllocator>, TreeTableImplementation::RedBlackTree> table;

		table.Insert(new int{ 1 });
		table.Insert(new int{ 2 });
		table.ForEach([](int* v) { printf("%d\n", *v); });
		table.ForEachDelete();
	}

	{
		TreeTable<ParameterPack_t<int, Comparator<int>, DefaultAllocator>, TreeTableImplementation::RedBlackTree> table;

		for (int i = 9000; i >= 0; i--)
			table.Insert(i);

		printf("%d\n", *table.LowerBound(500));
		printf("%d\n", *table.UpperBound(500));
		table.Remove(500);
		printf("%d\n", *table.LowerBound(500));
		printf("%d\n", *table.UpperBound(500));

		for (int i = 9000; i >= 0; i--)
			table.Insert(i);
	}
	
	return 0;
}

