/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 11:31:52 AM
 * =====================
 * 수동으로 Reset 해주지 않으면 계속 Signaled 상태인 이벤트 객체
 */


#pragma once

#include <JCore/Sync/WaitHandle.h>

NS_JC_BEGIN

class ManualResetEvent : public WaitHandle
{
public:
    ManualResetEvent(bool initialState, const char* name = nullptr): WaitHandle(initialState, true, name) {}
    ManualResetEvent(const ManualResetEvent& other) = delete;   // 복사 생성 금지
    ManualResetEvent(ManualResetEvent&& other) noexcept;

    ManualResetEvent& operator=(const ManualResetEvent& other) = delete; 
    ManualResetEvent& operator=(ManualResetEvent&& other) noexcept;
};

NS_JC_END 


