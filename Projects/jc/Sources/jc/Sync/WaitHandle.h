/*
 * 작성자: 윤정도
 * 생성일: 12/9/2022 2:21:35 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/String.h"
#include "jc/Primitives/SmartPtr.h"


NS_JC_BEGIN

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

	bool Wait(_u32 _timeout = JC_INFINITE, OUT _u32* _result = nullptr);
	bool Signal();
	bool Reset();
	const String& Name() { return name_; }

	void operator=(const WaitHandle& _other);
	void operator=(WaitHandle&& _other) noexcept;

public:
	static bool WaitAll(WaitHandle* _handles, _u32 _count, OUT_OPT _u32* _result = nullptr);
	static WaitHandle* WaitAny(WaitHandle* _handles, _u32 _count, OUT_OPT _u32* _result = nullptr);

	template <typename TCollection>
	static bool WaitAll(const TCollection& _handles, OUT_OPT _u32l* _result = nullptr)
	{
		jc_assert(_handles.Size() <= MAXIMUM_WAIT_OBJECTS && _handles.Size() > 0);
		_whandle waitHandles[MAXIMUM_WAIT_OBJECTS];

		auto it = _handles.Begin();
		int idx = 0;
		while (it.HasNext())
		{
			waitHandles[idx++] = it.Next().handle_;
		}
		const _u32l ret = WinApi::WaitForMultipleObjectsEx(_handles.Size(), waitHandles, true);

		if (_result)
			*_result = ret;

		return ret == WAIT_OBJECT_0;
	}

	template <typename TCollection>
	static WaitHandle* WaitAny(const TCollection& _handles)
	{
		jc_assert(_handles.Size() <= MAXIMUM_WAIT_OBJECTS && _handles.Size() > 0);
		_whandle waitHandles[MAXIMUM_WAIT_OBJECTS];

		auto iterator = _handles.Begin();
		int index = 0;
		while (iterator.HasNext())
		{
			waitHandles[index++] = iterator.Next().handle_;
		}

		_u32 waitResult = WinApi::WaitForMultipleObjectsEx(_handles.Size(), waitHandles, false);

		if (waitResult >= WAIT_OBJECT_0 && waitResult <= WAIT_OBJECT_0 + _handles.Size() - 1)
		{
			auto it = _handles.Begin();
			const int target = static_cast<int>(waitResult - WAIT_OBJECT_0);
			for (int i = 0; i < target; ++i)
			{
				it.Next();
			}
			return &it.Current();
		}

		return nullptr;
	}

protected:
	_whandle handle_;
	String name_;
};

using WaitHandlePtr = jc::SharedPtr<WaitHandle>;

NS_END
