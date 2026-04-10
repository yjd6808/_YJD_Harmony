
#define HEAP_STANDARD		0
#define HEAP_LAL			1
#define HEAP_LFH			2

auto PrintHeaps(HANDLE* _heaps, int _heapCount, bool _showEntry, const char* _pheader, const char* _fileName) -> void
{
	struct HeapStartEntry
	{
		PROCESS_HEAP_ENTRY base_;
		vector<PROCESS_HEAP_ENTRY> entries_;
	};

	vector<PROCESS_HEAP_ENTRY> entries;
	vector<HeapStartEntry> heapEntries;

	for (DWORD h = 0; h < _heapCount; ++h)
	{
		_ptr heapBase = (_ptr)_heaps[h];

		PROCESS_HEAP_ENTRY enumEntry{};
		enumEntry.lpData = nullptr;

		ULONG HeapCompatibility = 0;
		SIZE_T size = sizeof(HeapCompatibility);

		HeapQueryInformation(
			(HANDLE)heapBase,
			HeapCompatibilityInformation,
			&HeapCompatibility,
			sizeof(HeapCompatibility),
			nullptr);

		while (HeapWalk(_heaps[h], &enumEntry))
		{
			if (enumEntry.wFlags & PROCESS_HEAP_REGION)
			{
				if (enumEntry.Region.lpFirstBlock)
				{
					entries.push_back({});
					memcpy(&entries.back(), &enumEntry, sizeof(PROCESS_HEAP_ENTRY));
				}
			}
		}

		ranges::sort(entries, [](const PROCESS_HEAP_ENTRY& a, const PROCESS_HEAP_ENTRY& b)
		{
			return a.Region.lpFirstBlock < b.Region.lpFirstBlock;
		});

		for (const auto& et : entries)
		{
			heapEntries.push_back({});
			HeapStartEntry& startEntry = heapEntries.back();

			memcpy(&startEntry.base_, &et, sizeof(PROCESS_HEAP_ENTRY));
			memcpy(&enumEntry, &startEntry.base_, sizeof(PROCESS_HEAP_ENTRY));
			while (HeapWalk(_heaps[h], &enumEntry))
			{
				if (enumEntry.wFlags & PROCESS_HEAP_REGION)
					break;

				startEntry.entries_.push_back({});
				memcpy(&startEntry.entries_.back(), &enumEntry, sizeof(PROCESS_HEAP_ENTRY));
			}

			std::sort(startEntry.entries_.begin(), startEntry.entries_.end(), [](const PROCESS_HEAP_ENTRY& a, const PROCESS_HEAP_ENTRY& b)
			{
				return a.lpData < b.lpData;
			});
			
			
		}

		entries.clear();
	}

	String content;
	content += "==================================================\n";
	content += "[";
	content += _pheader;
	content += "]\n";
	for (int i = 0; i < heapEntries.size(); ++i)
	{
		HeapStartEntry& heap = heapEntries[i];
		String heapRange = StringUtil::Format("[%2d] Heap Base: 0x%p\n", i, heap.base_.lpData);
		heapRange += StringUtil::Format("BlockFirst: 0x%p, BlockLast: 0x%p, Commit: 0x%6x, Uncommit: 0x%4x, EntryCount: %3d\n",
			heap.base_.Region.lpFirstBlock,
			heap.base_.Region.lpLastBlock,
			heap.base_.Region.dwCommittedSize,
			heap.base_.Region.dwUnCommittedSize,
			(int)heap.entries_.size());

		if (_showEntry == false)
		{
			content += heapRange;
			continue;
		}
		for (int j =0; j < heap.entries_.size(); ++j)
		{
			const auto& et = heap.entries_[j];
			heapRange += StringUtil::Format("   [%4d] Data: 0xp%p, Size: 0x%5x, Overhead: 0x%4x, Flags: 0x%x (%s)\n",
				j,
				et.lpData,
				et.cbData,
				et.cbOverhead,
				et.wFlags,
				et.wFlags & PROCESS_HEAP_ENTRY_BUSY ? "busy" : "free");
		}
		
		content += heapRange;
	}
	Console::Write(content.SafeSource());

	File::WriteAllText(content, _fileName);
}

int call_02_HeapStudy(int _argc, char** _argv)
{
	constexpr int STD_CNT = 1000;
	HANDLE hLFHHeap = HeapCreate(0, 1024 * 1024 * 10, 0);
	char* lfhBlocks[STD_CNT]{};

	// LFH ON
	ULONG enableLFH = 2;
	HeapSetInformation(hLFHHeap, HeapCompatibilityInformation, &enableLFH, sizeof(enableLFH));

	// 같은 방식으로 할당
	for (int i = 0; i < STD_CNT; ++i)
	{
		lfhBlocks[i] = (char*)HeapAlloc(hLFHHeap, 0, i % 2 == 0 ? 0x100 + i : 0xff);
	}

	// 힙 상태 출력 (할당 직후)
	PrintHeaps(&hLFHHeap, 1, false, "LFH_BEFORE", "heap_lfh_before.txt");

	// 일부 블록 해제 후 작은 블록 재할당
	for (int i = 0; i < STD_CNT; i += 2)
	{
		HeapFree(hLFHHeap, 0, lfhBlocks[i]);
		lfhBlocks[i] = (char*)HeapAlloc(hLFHHeap, 0, 0x30);
	}

	// 힙 상태 출력 (해제/재할당 후)
	PrintHeaps(&hLFHHeap, 1, false, "LFH_AFTER", "heap_lfh_after.txt");

	HeapDestroy(hLFHHeap);
	return 0;
}
