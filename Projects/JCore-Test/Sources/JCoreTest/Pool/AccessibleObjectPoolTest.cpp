/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 3:53:56 PM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>

#include <JCore/Random.h>
#include <JCore/Pool/AccessibleObjectPool.h>

#include <JCore/Threading/ThreadPool.h>




using namespace std;

#if TEST_AccessibleObjectPoolTest == ON


class Data : public AccessibleObjectPool<Data>
{
	friend class TObjectPool;
	Data() {}
public:
	
	void OnPopped() override {}
};

constexpr int ThreadSize = 16;

TEST(AccessibleObjectPoolTest, General) {
	Random::EngineInitialize();
	Data::InitPool(1000, 10);
	ThreadPool th(ThreadSize);
	Task<void> tasks[ThreadSize];
	Tuple<bool, Data*> maps[20000]{};
	AtomicInt pushCount[ThreadSize]{};

	for (int i = 0; i < ThreadSize; ++i) {
		tasks[i] = th.Run([&, i] {
			for (int j = 0; j < 1000; ++j) {
				Data* data = Data::Pop();
				const int id = data->GetAccessId();
				EXPECT_FALSE(maps[id].item1);		// 중복된 ID 발급 확인

				// 40%확률로 데이터를 도로 집어넣는다.
				if (Random::Chance(40.0f)) {
					Data::Push(data);
					++pushCount[i];
				} else {
					maps[id].item1 = true;
					maps[id].item2 = data;
				}
			}
		});
	}

	// push를 수행한 횟수
	int totalPushCount = 0;

	for (int i = 0; i < ThreadSize; ++i) {
		tasks[i].Wait();
	}

	for (int i = 0; i < ThreadSize; ++i) {
		totalPushCount += pushCount[i];
	}



	int totalCount = Data::GetTotalCount();			// 실제할당된 객체 수
	int releasedCount = Data::GetRelasedCount();	// 반환된 객체 수
	int operatedCount = totalCount + totalPushCount - releasedCount;	// for 반복문을 몇번돌았는지 엘리먼트 수를 확인

	EXPECT_TRUE(ThreadSize * 1000 == operatedCount);

		for (int i = 0; i < ThreadSize; ++i) {
		tasks[i] = th.Run([&, i] {
			const int start = 1000 * i;
			const int end = 1000 * (i + 1);

			for (int j = start; j < end; ++j) {
				if (maps[j].item1) {
					Data::Push(maps[j].item2);
					maps[j].item1 = false;
					maps[j].item2 = nullptr;
					++totalPushCount;
				}
			}
		});
	}

	for (int i = 0; i < ThreadSize; ++i) {
		tasks[i].Wait();
	}

	releasedCount = Data::GetRelasedCount();
	totalCount = Data::GetTotalCount();

	EXPECT_TRUE(releasedCount == totalCount);

	// 전부 다시 풀로 복귀
	Data::FreeAllObjects();

	th.Join(ThreadPool::JoinStrategy::WaitAllTasks);

}

#endif
