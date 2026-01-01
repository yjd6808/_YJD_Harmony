/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 6:25:16 PM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/TypeCast.h>
#include <jc/Sync/ManualResetEvent.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ManualResetEvent::ManualResetEvent(ManualResetEvent&& _other) noexcept
	: WaitHandle(Move(_other))
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ManualResetEvent& ManualResetEvent::operator=(ManualResetEvent&& _other) noexcept
{
	WaitHandle::operator=(Move(_other));
	return *this;
}

NS_JC_END


