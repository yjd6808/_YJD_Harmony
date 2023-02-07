/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 6:25:16 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/TypeCast.h>
#include <JCore/Sync/ManualResetEvent.h>

NS_JC_BEGIN
ManualResetEvent::ManualResetEvent(ManualResetEvent&& other) noexcept : WaitHandle(Move(other)) {}
ManualResetEvent& ManualResetEvent::operator=(ManualResetEvent&& other) noexcept {
    WaitHandle::operator=(Move(other));
    return *this;
}
NS_JC_END


