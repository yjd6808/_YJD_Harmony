#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/NormalRwLock.h>

#if TEST_NormalRwLockTest == ON

namespace NormalRwLockTest {

static int number = 0;
static std::atomic<bool> detectInvalid;
static NormalRwLock lk;

TEST(NormalRwLockTest, NormalRwLock) {
    constexpr int maxThread = 5;
    std::vector<std::thread> vec;

    // 1개는 쓰기
    // 4개는 읽기

    // increser
    vec.emplace_back([] {
        for (int i = 0; i < 200'000; ++i) {
            lk.WriteLock();
            number++;
            number++;
            number++;
            number++;
            number++;
            number++;
            number++;
            number++;
            lk.WriteUnlock();
        }
    });

    for (int i = 1; i < maxThread; ++i) {
        vec.emplace_back([] {
            for (int j = 0; j < 200'000; ++j) {
                lk.ReadLock();
                if (number % 8 != 0) {
                    detectInvalid = true;
                }
                lk.ReadUnlock();
            }
        });
    }

    for (int i = 0; i < maxThread; ++i) {
        vec[i].join();
    }

    EXPECT_FALSE(detectInvalid);
    EXPECT_EQ(number, 1'600'000);
}

} // namespace NormalRwLockTest

#endif // #if TEST_NormalRwLockTest == ON