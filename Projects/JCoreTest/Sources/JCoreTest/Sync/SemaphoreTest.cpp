#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/Semaphore.h>

#if TEST_SemaphoreTest == ON

namespace SemaphoreTest {

static std::atomic<int> progress;
static std::atomic<bool> isGenerated;
static std::atomic<int> producedItemCount;  
static Semaphore writer(4, 4);
static Semaphore reader(1, 0);
static std::mutex mtx;

TEST(SemaphoreTest, Semaphore) {
    constexpr int maxThread = 5;
    std::vector<std::thread> vec;

    constexpr int writerCount = 4;
    constexpr int readerCount = 1;

    // 4개 생산자
    for (int i = 0; i < writerCount; i++) {
        vec.emplace_back([] {
            for (int j = 0; j < 200'000; j++) {
                writer.Acquire();
                std::unique_lock lg(mtx);
                ++progress;
                if (progress == writerCount)
                {
                    isGenerated = true;
                    progress = 0;
                    lg.unlock();
                    reader.Release();
                }
            }
        });
    }

    // 1개 소비자
    for (int i = 0; i < readerCount; i++) {
        vec.emplace_back([] {
            for (int j = 0; j < 200'000; j++) {
                reader.Acquire();
                if (isGenerated) ++producedItemCount;
                isGenerated = false;
                writer.Release(4);
            }
        });
    }

    for (int i = 0; i < maxThread; i++)
        vec[i].join();

    EXPECT_TRUE(producedItemCount == 200'000);
}

} // namespace SemaphoreTest

#endif // #if TEST_SemaphoreTest == ON