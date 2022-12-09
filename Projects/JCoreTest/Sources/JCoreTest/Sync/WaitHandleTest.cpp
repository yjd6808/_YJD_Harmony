/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 6:14:18 PM
 * =====================
 *
 */



#include <JCoreTest/CoreTest.h>
#include <JCore/Container/Vector.h>
#include <JCore/Sync/WaitHandle.h>

#if TEST_WaitHandleTest == ON

namespace WaitHandleTest {

Vector<WaitHandle> handles;

TEST(WaitHandleTest, WaitHandle) {
    handles.PushBack(WaitHandle{ false, true, "A"});
    handles.PushBack(WaitHandle{ false, true, "B"});
    handles.PushBack(WaitHandle{ false, true, "C"});
    handles.PushBack(WaitHandle{ false, true, "D"});
    handles.PushBack(WaitHandle{ false, true, "E"});
    handles.PushBack(WaitHandle{ false, true, "F"});

    std::thread th([] {
        this_thread::sleep_for(10ms);
        handles[0].Signal();

        this_thread::sleep_for(10ms);
        handles.Extension().ForEach([](WaitHandle& handle) { handle.Signal();});

        this_thread::sleep_for(10ms);
        handles[0].Signal();
        handles[handles.Size() - 1].Signal();
    });

    EXPECT_TRUE(handles[0].Wait());
    EXPECT_TRUE(handles[0].Reset());
    EXPECT_TRUE(WaitHandle::WaitAll(handles));
    EXPECT_TRUE(WaitHandle::WaitAll(handles)); 
    EXPECT_TRUE(WaitHandle::WaitAll(handles));

    handles.Extension().ForEach([](WaitHandle& handle) { handle.Reset(); });
    auto wakeup = WaitHandle::WaitAny(handles);
    EXPECT_TRUE(wakeup->Name() == "A");
    wakeup->Reset();

    EXPECT_TRUE(WaitHandle::WaitAny(handles)->Name() == "F");
    WaitHandle movedHandle(Move(handles[0]));

    EXPECT_TRUE(movedHandle.Name() == "A");
    EXPECT_TRUE(handles[0].Name().IsNull());
}

} // namespace NormalLockTest

#endif // #if TEST_NormalLockTest == ON