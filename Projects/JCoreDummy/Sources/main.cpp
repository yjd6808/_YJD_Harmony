#include <header.h>
#include <queue>

#include <timeapi.h>


int main() {
	MemoryPoolSingleFullIndexing s(false);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int t;
	//_CrtSetBreakAlloc(334);
	for (int i = 262000; i <= MemoryPoolSingleFullIndexing::MaxAllocatableSize; ++i) {
		void* mem = s.DynamicPop(i, t);
		s.DynamicPush(mem, t);
		

	}
	return 0;
}
