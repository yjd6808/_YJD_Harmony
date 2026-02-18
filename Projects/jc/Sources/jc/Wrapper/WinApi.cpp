/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:55 PM
 * =====================
 *
 */


#include <jc/Core.h>
#include <jc/Limit.h>
#include <jc/Wrapper/WinApi.h>

NS_JC_BEGIN
WinHandle WinApi::InvalidHandleValue = INVALID_HANDLE_VALUE;

bool
JC_CDECL
WinApi::SetConsoleCursorPosition( WinHandle _stdoutHandle,  int _x,  int _y)
{
	COORD p;
	p.X = static_cast<Int16>(_x);
	p.Y = static_cast<Int16>(_y);
	return ::SetConsoleCursorPosition(_stdoutHandle, p) != 0;
}

bool
JC_CDECL
WinApi::GetConsoleCursorPosition( WinHandle _stdoutHandle, OUT int& _x, OUT int& _y)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;

	if (::GetConsoleScreenBufferInfo(_stdoutHandle, &cbsi))
	{
		_x = cbsi.dwCursorPosition.X;
		_y = cbsi.dwCursorPosition.Y;
		return true;
	}

	return false;
}

bool
JC_CDECL
WinApi::SetConsoleTextAttribute( WinHandle _stdoutHandle,  Int16 _attribute)
{
	return ::SetConsoleTextAttribute(_stdoutHandle, _attribute) != 0;
}


bool
JC_CDECL
WinApi::SetConsoleOutputCodePage( Int _codePage)
{
	return ::SetConsoleOutputCP(_codePage) != 0;
}


Int
JC_CDECL
WinApi::GetConsoleOutputCodePage()
{
	return static_cast<int>(::GetConsoleOutputCP());
}

WinHandle
JC_CDECL
WinApi::GetStdoutHandle()
{
	return ::GetStdHandle(STD_OUTPUT_HANDLE);
}

WinHandle
JC_CDECL
WinApi::GetStdinHandle()
{
	return ::GetStdHandle(STD_INPUT_HANDLE);
}

WinHandle
JC_CDECL
WinApi::CreateEventA(bool _initialState, bool _manualReset, const char* _pName)
{
	return ::CreateEventA(nullptr, _manualReset ? TRUE : FALSE, _initialState ? TRUE : FALSE, _pName);
}

Int32UL
JC_CDECL
WinApi::WaitForMultipleObjectsEx( Int32U _eventCount,  WinHandle* _handles,  bool _waitAll,
                                  Int32U _timeout /*= JC_INFINITE */,  bool _alertable /*= false */)
{
	BOOL bWaitAll = _waitAll ? TRUE : FALSE;
	BOOL bAlertable = _alertable ? TRUE : FALSE;

	// 직접 호출 테스트
	DWORD result = ::WaitForMultipleObjectsEx(_eventCount, _handles, bWaitAll, _timeout, bAlertable);
	return result;
}

Int32UL
JC_CDECL
WinApi::WaitForSingleObject(WinHandle _handle, Int32U _timeout)
{
	return ::WaitForSingleObject(_handle, _timeout);
}

Int32UL
JC_CDECL
WinApi::GetLastError()
{
	return ::GetLastError();
}

bool
JC_CDECL
WinApi::SetEvent( WinHandle _handle)
{
	return ::SetEvent(_handle) != 0;
}

bool
JC_CDECL
WinApi::ResetEvent( WinHandle _handle)
{
	return ::ResetEvent(_handle) != 0;
}

bool
JC_CDECL
WinApi::CloseHandle( WinHandle _handle)
{
	return ::CloseHandle(_handle) != 0;
}

int
JC_CDECL
WinApi::GetThreadPriority( WinHandle _threadHandle)
{
	return ::GetThreadPriority(_threadHandle);
}

bool
JC_CDECL
WinApi::SetThreadPriority( WinHandle _threadHandle,  int _priority)
{
	return ::SetThreadPriority(_threadHandle, _priority) != 0;
}


Int32U
JC_CDECL
WinApi::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

Int32U
JC_CDECL
WinApi::GetModuleFilePath(WinModule _module, char* _filenameBuffer, int _filenameBufferCapacity)
{
	return GetModuleFileNameA((HMODULE)_module, _filenameBuffer, _filenameBufferCapacity);
}


// =========================================================================================
// Interlocked Api 
// =========================================================================================


template <typename TOperand>
TOperand
Interlocked<TOperand>::Add(IN_OUT TOperand* _destination,  TOperand _value)
{
	return ExchangeAdd(_destination, _value) + _value;
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::CompareExchange(IN_OUT TOperand* _pDestination,  TOperand _expected, TOperand _desired)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(::_InterlockedCompareExchange8(
			reinterpret_cast<volatile Boundary8*>(_pDestination),_desired, _expected));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedCompareExchange16(
			reinterpret_cast<volatile Boundary16*>(_pDestination), _desired, _expected));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedCompareExchange(
			reinterpret_cast<volatile Boundary32*>(_pDestination),_desired, _expected));
	}

	return static_cast<TOperand>(::_InterlockedCompareExchange64(
		reinterpret_cast<volatile Boundary64*>(_pDestination), _desired, _expected));
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Exchange(IN_OUT TOperand* _pDestination,  TOperand _value)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(
			::_InterlockedExchange8(reinterpret_cast<volatile Boundary8*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedExchange16(
			reinterpret_cast<volatile Boundary16*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(
			::_InterlockedExchange(reinterpret_cast<volatile Boundary32*>(_pDestination), _value));
	}

#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedExchange64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#else
	return static_cast<TOperand>(::_InlineInterlockedExchange64(
		reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::ExchangeAdd(IN_OUT TOperand* _pDestination,  TOperand _value)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(
			_InterlockedExchangeAdd8(reinterpret_cast<volatile Boundary8*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(_InterlockedExchangeAdd16(
			reinterpret_cast<volatile Boundary16*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedExchangeAdd(
			reinterpret_cast<volatile Boundary32*>(_pDestination), _value));
	}

#ifdef _WIN64
	return static_cast<TOperand>(
		::_InterlockedExchangeAdd64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#else
	return static_cast<TOperand>(::_InlineInterlockedExchangeAdd64(
		reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Increment(IN_OUT TOperand* _destination)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return Add(_destination, 1);
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedIncrement16(reinterpret_cast<volatile Boundary16*>(_destination)));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedIncrement(reinterpret_cast<volatile Boundary32*>(_destination)));
	}
#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(_destination)));
#else
	return static_cast<TOperand>(::_InlineInterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(_destination)));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Decrement(IN_OUT TOperand* _pDestination)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return Add(_pDestination, -1);
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedDecrement16(reinterpret_cast<volatile Boundary16*>(_pDestination)));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedDecrement(reinterpret_cast<volatile Boundary32*>(_pDestination)));
	}

#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedDecrement64(reinterpret_cast<volatile Boundary64*>(_pDestination)));
#else
	return static_cast<TOperand>(::_InlineInterlockedDecrement64(reinterpret_cast<volatile Boundary64*>(_pDestination)));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::And(IN_OUT TOperand* _destination,  TOperand _value)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(::_InterlockedAnd8(reinterpret_cast<volatile Boundary8*>(_destination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedAnd16(reinterpret_cast<volatile Boundary16*>(_destination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedAnd(reinterpret_cast<volatile Boundary32*>(_destination), _value));
	}

#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedAnd64(reinterpret_cast<volatile Boundary64*>(_destination), _value));
#else
	return static_cast<TOperand>(::_InlineInterlockedAnd64(reinterpret_cast<volatile Boundary64*>(_destination), _value));
#endif
}

template <typename TOperand>
TOperand
Interlocked<TOperand>::Or(IN_OUT TOperand* _pDestination,  TOperand _value)
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(::_InterlockedOr8(reinterpret_cast<volatile Boundary8*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedOr16(reinterpret_cast<volatile Boundary16*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedOr(reinterpret_cast<volatile Boundary32*>(_pDestination), _value));
	}

#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedOr64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#else
	return static_cast<TOperand>(::_InlineInterlockedOr64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#endif
}

template <typename TOperand>
auto Interlocked<TOperand>::Xor(IN_OUT TOperand* _pDestination,  TOperand _value) -> TOperand
{
	if constexpr (sizeof(TOperand) == sizeof(Boundary8))
	{
		return static_cast<TOperand>(::_InterlockedXor8(reinterpret_cast<volatile Boundary8*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary16))
	{
		return static_cast<TOperand>(::_InterlockedXor16(reinterpret_cast<volatile Boundary16*>(_pDestination), _value));
	}

	if constexpr (sizeof(TOperand) == sizeof(Boundary32))
	{
		return static_cast<TOperand>(::_InterlockedXor(reinterpret_cast<volatile Boundary32*>(_pDestination), _value));
	}

#ifdef _WIN64
	return static_cast<TOperand>(::_InterlockedXor64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#else
	return static_cast<TOperand>(::_InlineInterlockedXor64(reinterpret_cast<volatile Boundary64*>(_pDestination), _value));
#endif
}


bool
Interlocked<bool>::CompareExchange(IN_OUT bool* _pDestination,  bool _expected,  bool _desired)
{
	Boundary8 iExpected = _expected ? 1 : 0;
	Boundary8 iDesired = _desired ? 1 : 0;

	return TInterlocked::CompareExchange(reinterpret_cast<Boundary8*>(_pDestination), iExpected, iDesired) ? true : false;
}

bool
Interlocked<bool>::Exchange(bool* _pDestination, bool _value)
{
	Boundary8 iValue = _value ? 1 : 0;
	return TInterlocked::Exchange(reinterpret_cast<Boundary8*>(_pDestination), iValue) ? true : false;
}

bool Interlocked<bool>::Read(bool* _destination)
{
	return TInterlocked::Read(reinterpret_cast<Boundary8*>(_destination));
}

template struct Interlocked<Int8>;
template struct Interlocked<Int8U>;
template struct Interlocked<Int16>;
template struct Interlocked<Int16U>;
template struct Interlocked<WideChar>;
template struct Interlocked<Int32>;
template struct Interlocked<Int32U>;
template struct Interlocked<Int32L>;
template struct Interlocked<Int32UL>;
template struct Interlocked<Int64>;
template struct Interlocked<Int64U>;
template struct Interlocked<bool>;

NS_END
