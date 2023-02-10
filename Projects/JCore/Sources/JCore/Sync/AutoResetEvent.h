/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 5:29:04 PM
 * =====================
 * Signal되면 자동으로 Non-Signaled 상태로 전환되는 이벤트 객체
 */


#pragma once

#pragma once

#include <JCore/Sync/WaitHandle.h>

NS_JC_BEGIN

class AutoResetEvent : public WaitHandle
{
public:
    AutoResetEvent(bool initialState, const char* name = nullptr) : WaitHandle(initialState, true, name) {}
    AutoResetEvent(const AutoResetEvent& other) = delete;
    AutoResetEvent(AutoResetEvent&& other) noexcept;
    ~AutoResetEvent() override = default;

    AutoResetEvent& operator=(const AutoResetEvent& other) = delete; 
    AutoResetEvent& operator=(AutoResetEvent&& other) noexcept;
};

NS_JC_END 