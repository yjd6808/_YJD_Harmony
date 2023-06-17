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

NS_JC_BEGIN

WaitHandle::WaitHandle(bool initialState, bool manualReset, const char* name)
	: m_hHandle(WinApi::CreateEventA(initialState, manualReset, name))
	, m_Name(name)
{}

WaitHandle::WaitHandle(WaitHandle&& handle) noexcept {
   this->operator=(Move(handle));
}

WaitHandle::~WaitHandle() {
    if (m_hHandle) {
        WinApi::CloseHandle(m_hHandle);
    }
}

bool WaitHandle::Wait(Int32U timeout, JCORE_OUT Int32U* result) {
    if (m_hHandle == nullptr)
        return false;

    const Int32U uiResult = WinApi::WaitForMultipleObjectsEx(1, &m_hHandle, true, timeout);

    if (result)
        *result = uiResult;
    
    return uiResult == WAIT_OBJECT_0;
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
bool WaitHandle::WaitAll(WaitHandle* handles, Int32U count, JCORE_OUT_OPT Int32U* result) {
    DebugAssert(count <= MAXIMUM_WAIT_OBJECTS);
    WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];
    for (Int32U i = 0; i < count; ++i) {
        waitHandles[i] = handles[i].m_hHandle;
    }

    const Int32UL uiResult = WinApi::WaitForMultipleObjectsEx(count, waitHandles, true);

    if (result)
        *result = uiResult;

    if (uiResult == WAIT_OBJECT_0) {
        return true;
    }

    return false;
}



WaitHandle* WaitHandle::WaitAny(WaitHandle* handles, Int32U count, JCORE_OUT_OPT Int32U* result) {
    DebugAssert(count <= MAXIMUM_WAIT_OBJECTS);
    WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

    for (Int32U i = 0; i < count; ++i) {
        waitHandles[i] = handles[i].m_hHandle;
    }

    const Int32U uiResult = WinApi::WaitForMultipleObjectsEx(count, waitHandles, false);

    if (result)
        *result = uiResult;

    if (uiResult >= WAIT_OBJECT_0 && uiResult <= WAIT_OBJECT_0 + count - 1)
        return &handles[uiResult - WAIT_OBJECT_0];

    return nullptr;
}


  
NS_JC_END