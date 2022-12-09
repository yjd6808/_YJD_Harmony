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

namespace JCore {

    template <typename T>
    class Collection;
    class WaitHandle
    {
    public:
        WaitHandle(): m_hHandle(nullptr), m_Name(0) {}
        WaitHandle(bool initialState, bool manualReset, const char* name = nullptr);
        WaitHandle(const WaitHandle& handle) = delete;      // 복사 생성을 금한다.
        WaitHandle(WaitHandle&& handle) noexcept;           // 이동 생성만 허용
        ~WaitHandle();

        bool Wait(Int32U timeout = JCoreInfinite);
        bool Signal();
        bool Reset();
        String Name() const { return m_Name; }

        void operator=(const WaitHandle& other) = delete;
        void operator=(WaitHandle&& other) noexcept;
    public:
        static bool WaitAll(WaitHandle* handles, Int32U count);
        static bool WaitAll(Collection<WaitHandle>& handles);
        static WaitHandle* WaitAny(WaitHandle* handles, Int32U count);
        static WaitHandle* WaitAny(Collection<WaitHandle>& handles);
    protected:
        WinHandle m_hHandle;
        String m_Name;
    };
}

