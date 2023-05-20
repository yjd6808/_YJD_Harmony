/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 2:21:35 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/SmartPtr.h>


NS_JC_BEGIN

template <typename, typename>
class Collection;
class WaitHandle
{
public:
    WaitHandle(): m_hHandle(nullptr), m_Name(0) {}
    WaitHandle(bool initialState, bool manualReset, const char* name = nullptr);
    WaitHandle(const WaitHandle& handle) = delete;      // 복사 생성을 금한다.
    WaitHandle(WaitHandle&& handle) noexcept;           // 이동 생성만 허용
    virtual ~WaitHandle();

    bool Wait(Int32U timeout = JCORE_INFINITE, JCORE_OUT Int32U* result = nullptr);
    bool Signal();
    bool Reset();
    const String& Name() { return m_Name; }

    void operator=(const WaitHandle& other) = delete;
    void operator=(WaitHandle&& other) noexcept;
public:
    static bool WaitAll(WaitHandle* handles, Int32U count, JCORE_OUT_OPT Int32U* result = nullptr);
    static WaitHandle* WaitAny(WaitHandle* handles, Int32U count, JCORE_OUT_OPT Int32U* result = nullptr);

    template <typename TAllocator>
	static bool WaitAll(Collection<WaitHandle, TAllocator>& handles) {
	    DebugAssert(handles.Size() <= MAXIMUM_WAIT_OBJECTS && handles.Size() > 0);
	    WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

	    auto it = handles.Begin();
	    int idx = 0;
	    while (it->HasNext()) {
	        waitHandles[idx++] = it->Next().m_hHandle;
	    }

	    return WinApi::WaitForMultipleObjectsEx(handles.Size(), waitHandles, true) == WAIT_OBJECT_0;
	}

	template <typename TAllocator>
	static WaitHandle* WaitAny(Collection<WaitHandle, TAllocator>& handles) {
        DebugAssert(handles.Size() <= MAXIMUM_WAIT_OBJECTS && handles.Size() > 0);
        WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

        auto it = handles.Begin();
        int idx = 0;
        while (it->HasNext()) {
            waitHandles[idx++] = it->Next().m_hHandle;
        }

        Int32U iResult = WinApi::WaitForMultipleObjectsEx(handles.Size(), waitHandles, false);

        if (iResult >= WAIT_OBJECT_0 && iResult <= WAIT_OBJECT_0 + handles.Size() - 1)
            return handles.Extension().IndexOf(static_cast<int>(iResult - WAIT_OBJECT_0));

        return nullptr;
    }
    

protected:
    WinHandle m_hHandle;
    String m_Name;
};

using WaitHandlePtr = JCore::SharedPtr<WaitHandle>;

NS_JC_END
