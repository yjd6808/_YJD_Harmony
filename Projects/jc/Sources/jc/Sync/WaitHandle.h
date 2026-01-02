/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 2:21:35 PM
 * =====================
 *
 */


#pragma once

#include <jc/Type.h>
#include <jc/Define.h>
#include <jc/Primitives/String.h>
#include <jc/Primitives/SmartPtr.h>


NS_JC_BEGIN
template <typename, typename>
class Collection;

class WaitHandle
{
public:
	WaitHandle()
	: handle_(nullptr), name_(0)
	{
	}

	WaitHandle(bool _initialState, bool _manualReset, const char* _name = nullptr);
	WaitHandle(const WaitHandle& _handle) = delete; // 복사 생성을 금한다.
	WaitHandle(WaitHandle&& _handle) noexcept; // 이동 생성만 허용
	virtual ~WaitHandle();

	bool Wait(Int32U _timeout = JC_INFINITE, OUT Int32U* _result = nullptr);
	bool Signal();
	bool Reset();
	const String& Name() { return name_; }

	void operator=(const WaitHandle& _other);
	void operator=(WaitHandle&& _other) noexcept;

public:
	static bool WaitAll(WaitHandle* _handles, Int32U _count, OUT_OPT Int32U* _result = nullptr);
	static WaitHandle* WaitAny(WaitHandle* _handles, Int32U _count, OUT_OPT Int32U* _result = nullptr);

	template <typename TCollection>
	static bool WaitAll(const TCollection& _handles, OUT_OPT Int32UL* _result = nullptr)
	{
		jc_assert(_handles.Size() <= MAXIMUM_WAIT_OBJECTS && _handles.Size() > 0);
		WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

		auto it = _handles.Begin();
		int idx = 0;
		while (it->HasNext())
		{
			waitHandles[idx++] = it->Next().handle_;
		}
		const Int32UL ret = WinApi::WaitForMultipleObjectsEx(_handles.Size(), waitHandles, true);

		if (_result)
			*_result = ret;

		return ret == WAIT_OBJECT_0;
	}

	template <typename TAllocator>
	static WaitHandle* WaitAny(Collection<WaitHandle, TAllocator>& _handles)
	{
		jc_assert(_handles.Size() <= MAXIMUM_WAIT_OBJECTS && _handles.Size() > 0);
		WinHandle waitHandles[MAXIMUM_WAIT_OBJECTS];

		auto iterator = _handles.Begin();
		int index = 0;
		while (iterator->HasNext())
		{
			waitHandles[index++] = iterator->Next().handle_;
		}

		Int32U waitResult = WinApi::WaitForMultipleObjectsEx(_handles.Size(), waitHandles, false);

		if (waitResult >= WAIT_OBJECT_0 && waitResult <= WAIT_OBJECT_0 + _handles.Size() - 1)
			return _handles.Extension().IndexOf(static_cast<int>(waitResult - WAIT_OBJECT_0));

		return nullptr;
	}

protected:
	WinHandle handle_;
	String name_;
};

using WaitHandlePtr = jc::SharedPtr<WaitHandle>;

NS_JC_END
