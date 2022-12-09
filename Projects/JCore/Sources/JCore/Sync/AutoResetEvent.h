/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 5:29:04 PM
 * =====================
 * Signal되면 자동으로 Non-Signaled 상태로 전환되는 이벤트 객체
 */


#pragma once

#pragma once

#include <JCore/Sync/WaitHandle.h>

namespace JCore {

    class AutoResetEvent : public WaitHandle
    {
    public:
        AutoResetEvent(bool initialState, const char* name = nullptr) : WaitHandle(initialState, true, name) {}
        AutoResetEvent(const AutoResetEvent& other) = delete;
        AutoResetEvent(AutoResetEvent&& other) noexcept;

        AutoResetEvent& operator=(const AutoResetEvent& other) = delete; 
        AutoResetEvent& operator=(AutoResetEvent&& other) noexcept;
    };

} // namespace JCore 