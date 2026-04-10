/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:55 PM
 * =====================
 *
 */


#include "jc/Limit.h"
#include "jc/Wrapper/WinApi.h"

NS_JC_BEGIN
_whandle WinApi::InvalidHandleValue = INVALID_HANDLE_VALUE;

bool
JC_CDECL
WinApi::SetConsoleCursorPosition( _whandle _stdoutHandle,  int _x,  int _y)
{
	COORD p;
	p.X = static_cast<_s16>(_x);
	p.Y = static_cast<_s16>(_y);
	return ::SetConsoleCursorPosition(_stdoutHandle, p) != 0;
}

bool
JC_CDECL
WinApi::GetConsoleCursorPosition( _whandle _stdoutHandle, OUT int& _x, OUT int& _y)
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
WinApi::SetConsoleTextAttribute( _whandle _stdoutHandle,  _s16 _attribute)
{
	return ::SetConsoleTextAttribute(_stdoutHandle, _attribute) != 0;
}


bool
JC_CDECL
WinApi::SetConsoleOutputCodePage( _s32 _codePage)
{
	return ::SetConsoleOutputCP(_codePage) != 0;
}


_s32
JC_CDECL
WinApi::GetConsoleOutputCodePage()
{
	return static_cast<int>(::GetConsoleOutputCP());
}

_whandle
JC_CDECL
WinApi::GetStdoutHandle()
{
	return ::GetStdHandle(STD_OUTPUT_HANDLE);
}

_whandle
JC_CDECL
WinApi::GetStdinHandle()
{
	return ::GetStdHandle(STD_INPUT_HANDLE);
}

_whandle
JC_CDECL
WinApi::CreateEventA(bool _initialState, bool _manualReset, const char* _pName)
{
	return ::CreateEventA(nullptr, _manualReset ? TRUE : FALSE, _initialState ? TRUE : FALSE, _pName);
}

_u32l
JC_CDECL
WinApi::WaitForMultipleObjectsEx( _u32 _eventCount,  _whandle* _handles,  bool _waitAll,
                                  _u32 _timeout /*= JC_INFINITE */,  bool _alertable /*= false */)
{
	BOOL bWaitAll = _waitAll ? TRUE : FALSE;
	BOOL bAlertable = _alertable ? TRUE : FALSE;

	// 직접 호출 테스트
	DWORD result = ::WaitForMultipleObjectsEx(_eventCount, _handles, bWaitAll, _timeout, bAlertable);
	return result;
}

_u32l
JC_CDECL
WinApi::WaitForSingleObject(_whandle _handle, _u32 _timeout)
{
	return ::WaitForSingleObject(_handle, _timeout);
}

_u32l
JC_CDECL
WinApi::GetLastError()
{
	return ::GetLastError();
}

bool
JC_CDECL
WinApi::SetEvent( _whandle _handle)
{
	return ::SetEvent(_handle) != 0;
}

bool
JC_CDECL
WinApi::ResetEvent( _whandle _handle)
{
	return ::ResetEvent(_handle) != 0;
}

bool
JC_CDECL
WinApi::CloseHandle( _whandle _handle)
{
	return ::CloseHandle(_handle) != 0;
}

int
JC_CDECL
WinApi::GetThreadPriority( _whandle _threadHandle)
{
	return ::GetThreadPriority(_threadHandle);
}

bool
JC_CDECL
WinApi::SetThreadPriority( _whandle _threadHandle,  int _priority)
{
	return ::SetThreadPriority(_threadHandle, _priority) != 0;
}


_u32
JC_CDECL
WinApi::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

_u32
JC_CDECL
WinApi::GetModuleFilePath(IN_OPT _wmodule _module, OUT char* _filenameBuffer, int _filenameBufferCapacity)
{
	return GetModuleFileNameA((HMODULE)_module, _filenameBuffer, _filenameBufferCapacity);
}

//////////////////////////////////////////////////////////////////////////////////////////
String JC_CDECL WinApi::GetMemoryBasicInformationString(const MEMORY_BASIC_INFORMATION& _mbi, int _index /*= 0*/)
{
	String allocProtectStr = WinApi::ProtectToString(_mbi.AllocationProtect);
	String stateStr = WinApi::MemoryStateToString(_mbi.State);
	String protectStr = WinApi::ProtectToString(_mbi.Protect);
	String typeStr = WinApi::MemoryTypeToString(_mbi.Type);

	String msg;
	msg.Format(
		"[%d] Base: 0x%p AllocBase: 0x%p\n"
		"AllocProtect: %s RegionSize: 0x%x State: %s Protect: %s Type: %s\n",
		_index, _mbi.BaseAddress, _mbi.AllocationBase,
		allocProtectStr.Source(), _mbi.RegionSize, stateStr.Source(), protectStr.Source(), typeStr.Source());
	return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
void WinApi::PrintMemoryBasicInformation(const MEMORY_BASIC_INFORMATION& _memInfo, int _index)
{
	Console::WriteLine("%s", GetMemoryBasicInformationString(_memInfo, _index).SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
String JC_CDECL WinApi::MemoryStateToString(DWORD _state)
{
	switch (_state)
	{
	case MEM_COMMIT:  return "MEM_COMMIT";
	case MEM_RESERVE: return "MEM_RESERVE";
	case MEM_FREE:    return "MEM_FREE";
	default:          return "UNKNOWN";
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String JC_CDECL WinApi::ProtectToString(DWORD _protect)
{
	String result(128);

	if (_protect == 0)
		return "0";

	if (_protect & PAGE_EXECUTE) result += "PAGE_EXECUTE|";
	if (_protect & PAGE_EXECUTE_READ) result += "PAGE_EXECUTE_READ|";
	if (_protect & PAGE_EXECUTE_READWRITE) result += "PAGE_EXECUTE_READWRITE|";
	if (_protect & PAGE_EXECUTE_WRITECOPY) result += "PAGE_EXECUTE_WRITECOPY|";
	if (_protect & PAGE_NOACCESS) result += "PAGE_NOACCESS|";
	if (_protect & PAGE_READONLY) result += "PAGE_READONLY|";
	if (_protect & PAGE_READWRITE) result += "PAGE_READWRITE|";
	if (_protect & PAGE_WRITECOPY) result += "PAGE_WRITECOPY|";

	if (_protect & PAGE_GUARD) result += "PAGE_GUARD|";
	if (_protect & PAGE_NOCACHE) result += "PAGE_NOCACHE|";
	if (_protect & PAGE_WRITECOMBINE) result += "PAGE_WRITECOMBINE|";

	if (!result.IsEmpty())
		result.PopBack();

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
String JC_CDECL WinApi::MemoryTypeToString(DWORD _type)
{
	switch (_type)
	{
	case MEM_IMAGE:   return "MEM_IMAGE";
	case MEM_MAPPED:  return "MEM_MAPPED";
	case MEM_PRIVATE: return "MEM_PRIVATE";
	default:          return "UNKNOWN";
	}
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

template struct Interlocked<_s8>;
template struct Interlocked<_u8>;
template struct Interlocked<_s16>;
template struct Interlocked<_u16>;
template struct Interlocked<_s16c>;
template struct Interlocked<_s32>;
template struct Interlocked<_u32>;
template struct Interlocked<_s32l>;
template struct Interlocked<_u32l>;
template struct Interlocked<_s64>;
template struct Interlocked<_u64>;
template struct Interlocked<bool>;

NS_END
