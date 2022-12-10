/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 2:24:27 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Wrapper/WinApi.h>
#include <JCore/Sync/WaitHandle.h>
#include <JCore/Container/Collection.h>

namespace JCore {
    WaitHandle::WaitHandle(bool initialState, bool manualReset, const char* name) :
        m_hHandle(WinApi::CreateEvent(initialState, manualReset, name)),
        m_Name(name),
        m_bSignaled(initialState)
    {}

    WaitHandle::WaitHandle(WaitHandle&& handle) noexcept {
       this->operator=(Move(handle));
    }

    WaitHandle::~WaitHandle() {
        if (m_hHandle) {
            WinApi::CloseHandle(m_hHandle);
        }
    }

    bool WaitHandle::Wait(Int32U timeout) {
        if (m_hHandle == nullptr)
            return false;

        return WinApi::WaitForMultipleObjectsEx(1, &m_hHandle, true) == WAIT_OBJECT_0;
    }

    bool WaitHandle::Signal() {
        if (m_hHandle == nullptr)
            return false;

        return WinApi::SetEvent(m_hHandle) != 0;
    }

    bool WaitHandle::Reset() {
        if (m_hHandle == nullptr)
            return false;

        return WinApi::ResetEvent(m_hHandle);
    }


    void WaitHandle::operator=(WaitHandle&& other) noexcept {
        m_hHandle = other.m_hHandle;
        other.m_hHandle = nullptr;

        if (other.m_Name.IsNull())
            return;

        m_Name = Move(other.m_Name);
    }

    // =====================================================================
    // static
    // =====================================================================
    bool WaitHandle::WaitAll(WaitHandle* handles, Int32U count) {
        DebugAssert(count <= MAXIMUM_WAIT_OBJECTS);
        WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];
        for (Int32U i = 0; i < count; ++i) {
            waitHandles[i] = handles[i].m_hHandle;
        }

        return WinApi::WaitForMultipleObjectsEx(count, waitHandles, true) == WAIT_OBJECT_0;
    }

    bool WaitHandle::WaitAll(Collection<WaitHandle>& handles) {
        DebugAssert(handles.Size() <= MAXIMUM_WAIT_OBJECTS && handles.Size() > 0);
        WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

        auto it = handles.Begin();
        int idx = 0;
        while (it->HasNext()) {
            waitHandles[idx++] = it->Next().m_hHandle;
        }

        return WinApi::WaitForMultipleObjectsEx(handles.Size(), waitHandles, true) == WAIT_OBJECT_0;
    }

    WaitHandle* WaitHandle::WaitAny(WaitHandle* handles, Int32U count) {
        DebugAssert(count <= MAXIMUM_WAIT_OBJECTS);
        WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

        for (Int32U i = 0; i < count; ++i) {
            waitHandles[i] = handles[i].m_hHandle;
        }

        Int32U iResult = WinApi::WaitForMultipleObjectsEx(count, waitHandles, false);

        if (iResult >= WAIT_OBJECT_0 && iResult <= WAIT_OBJECT_0 + count - 1)
            return &handles[iResult - WAIT_OBJECT_0];

        return nullptr;
    }

    WaitHandle* WaitHandle::WaitAny(Collection<WaitHandle>& handles) {
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
} // namespace JCore