#include <JCoreTest/CoreTest.h>
#include <JCore/Sync/SpinLock.h>

#if TEST_SpinLockTest == ON

namespace SpinLockTest {

static int number = 0;
static SpinLock lk;

TEST(SpinLockTest, SpinLock) {
    number = 0;

    std::vector<std::thread> vec;

    for (int i = 0; i < 8; i++) {
        vec.emplace_back([i] {
            for (int j = 0; j < 80'000; j++) {
                SpinLockGuard lg(lk);
                if (i < 4)
                    number++;
                else
                    number--;
            }
        });
    }

    for (int i = 0; i < 8; i++)
        vec[i].join();

    EXPECT_TRUE(number == 0);
}

} // namespace SpinLockTest

#endif // #if TEST_SpinLockTest == ON