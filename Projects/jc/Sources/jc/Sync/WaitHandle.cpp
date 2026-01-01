/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 2:24:27 PM
 * =====================
 *
 */


#include <jc/Core.h>
#include <jc/Wrapper/WinApi.h>
#include <jc/Sync/WaitHandle.h>
#include <jc/Container/Collection.h>

NS_JC_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
WaitHandle::WaitHandle(bool _initialState, bool _manualReset, const char* _name)
: handle_(WinApi::CreateEventA(_initialState, _manualReset, _name))
, name_(_name)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
WaitHandle::WaitHandle(WaitHandle&& _handle) noexcept
{
	this->operator=(Move(_handle));
}

//////////////////////////////////////////////////////////////////////////////////////////
WaitHandle::~WaitHandle()
{
	if (handle_)
	{
		WinApi::CloseHandle(handle_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool WaitHandle::Wait(Int32U _timeout, OUT Int32U* _pResult)
{
	if (handle_ == nullptr)
		return false;

	const Int32U waitResult = WinApi::WaitForMultipleObjectsEx(1, &handle_, true, _timeout);

	if (_pResult)
		*_pResult = waitResult;

	return waitResult == WAIT_OBJECT_0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool WaitHandle::Signal()
{
	if (handle_ == nullptr)
		return false;

	return WinApi::SetEvent(handle_) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool WaitHandle::Reset()
{
	if (handle_ == nullptr)
		return false;

	return WinApi::ResetEvent(handle_);
}

void WaitHandle::operator=(const WaitHandle& _other)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WaitHandle::operator=(WaitHandle&& _other) noexcept
{
	handle_ = _other.handle_;
	_other.handle_ = nullptr;

	if (_other.name_.IsNull())
		return;

	name_ = Move(_other.name_);
}

// =====================================================================
// static
// =====================================================================
//////////////////////////////////////////////////////////////////////////////////////////
bool WaitHandle::WaitAll(WaitHandle* _pHandles, Int32U _count, OUT_OPT Int32U* _pResult)
{
	jc_assert(_count <= MAXIMUM_WAIT_OBJECTS);
	WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];
	for (Int32U index = 0; index < _count; ++index)
	{
		waitHandles[index] = _pHandles[index].handle_;
	}

	const Int32UL waitResult = WinApi::WaitForMultipleObjectsEx(_count, waitHandles, true);

	if (_pResult)
		*_pResult = waitResult;

	if (waitResult == WAIT_OBJECT_0)
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
WaitHandle* WaitHandle::WaitAny(WaitHandle* _pHandles, Int32U _count, OUT_OPT Int32U* _pResult)
{
	jc_assert(_count <= MAXIMUM_WAIT_OBJECTS);
	WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

	for (Int32U index = 0; index < _count; ++index)
	{
		waitHandles[index] = _pHandles[index].handle_;
	}

	const Int32U waitResult = WinApi::WaitForMultipleObjectsEx(_count, waitHandles, false);

	if (_pResult)
		*_pResult = waitResult;

	if (waitResult >= WAIT_OBJECT_0 && waitResult <= WAIT_OBJECT_0 + _count - 1)
		return &_pHandles[waitResult - WAIT_OBJECT_0];

	return nullptr;
}


NS_JC_END
