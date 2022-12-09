/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 6:25:16 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/TypeCast.h>
#include <JCore/Sync/ManualResetEvent.h>

namespace JCore {
    ManualResetEvent::ManualResetEvent(ManualResetEvent&& other) noexcept : WaitHandle(Move(other)) {}
    ManualResetEvent& ManualResetEvent::operator=(ManualResetEvent&& other) noexcept {
        this->operator=(Move(other));
        return *this;
    }
} // namespace JCore


