/*
 * 작성자: 윤정도
 * 생성일: 7/26/2026
 * =====================
 */


#include "jc/Utils/StackTrace.h"
#include "jc/Logger/LoggerDefine.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <ctime>

#pragma comment(lib, "DbgHelp.lib")

NS_JC_BEGIN

bool StackTrace::s_bSymInit = false;
void* StackTrace::s_hProcess = GetCurrentProcess();

//////////////////////////////////////////////////////////////////////////////////////////
StackTrace::StackTrace()
	: frameCount_(0)
{
	for (int i = 0; i < MaxFrames; ++i)
		addresses_[i] = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
StackTrace::~StackTrace()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
static void BuildSearchPath(char* _pOut, int _capacity)
{
	char szExePath[MAX_PATH] = { 0, };
	GetModuleFileNameA(nullptr, szExePath, MAX_PATH);

	char szExeDir[MAX_PATH] = { 0, };
	char szDrive[_MAX_DRIVE] = { 0, };
	char szDir[_MAX_DIR] = { 0, };
	_splitpath_s(szExePath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, nullptr, 0, nullptr, 0);
	sprintf_s(szExeDir, "%s%s", szDrive, szDir);

	char szCurDir[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, szCurDir);

	char szNtSymPath[MAX_PATH] = { 0, };
	DWORD dwNtSymLen = GetEnvironmentVariableA("_NT_SYMBOL_PATH", szNtSymPath, MAX_PATH);

	if (dwNtSymLen > 0)
		sprintf_s(_pOut, _capacity, "%s;%s;%s", szExeDir, szCurDir, szNtSymPath);
	else
		sprintf_s(_pOut, _capacity, "%s;%s", szExeDir, szCurDir);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::EnsureSymInit()
{
	if (s_bSymInit)
		return;

	s_bSymInit = true;

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS);

	char szSearchPath[4096] = { 0, };
	BuildSearchPath(szSearchPath, sizeof(szSearchPath));

	if (!SymInitialize(s_hProcess, szSearchPath, TRUE))
	{
		_LogError_("SymInitialize 실패: GetLastError=%lu, SearchPath=%s", GetLastError(), szSearchPath);
	}

	char szExePath[MAX_PATH] = { 0, };
	GetModuleFileNameA(nullptr, szExePath, MAX_PATH);
	SymLoadModule64(s_hProcess, nullptr, szExePath, nullptr, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::Capture(int _skipFrames)
{
	frameCount_ = CaptureStackBackTrace(_skipFrames, MaxFrames, addresses_, nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::Capture(CONTEXT* _pContext)
{
	EnsureSymInit();

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

	STACKFRAME64 stackFrame = { 0, };
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Mode = AddrModeFlat;

#ifdef _M_AMD64
	stackFrame.AddrPC.Offset = _pContext->Rip;
	stackFrame.AddrFrame.Offset = _pContext->Rbp;
	stackFrame.AddrStack.Offset = _pContext->Rsp;
	DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
#else
	stackFrame.AddrPC.Offset = _pContext->Eip;
	stackFrame.AddrFrame.Offset = _pContext->Ebp;
	stackFrame.AddrStack.Offset = _pContext->Esp;
	DWORD machineType = IMAGE_FILE_MACHINE_I386;
#endif

	int frameCount = 0;
	CONTEXT contextCopy = *_pContext;

	while (frameCount < MaxFrames)
	{
		if (!StackWalk64(machineType, hProcess, hThread, &stackFrame, &contextCopy,
			nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
			break;

		if (stackFrame.AddrPC.Offset == 0)
			break;

		addresses_[frameCount++] = (void*)stackFrame.AddrPC.Offset;
	}
	frameCount_ = frameCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
static const char* GetModuleNameFromAddress(void* _pAddress)
{
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)_pAddress, &hModule);

	if (!hModule) return nullptr;

	static char szModName[MAX_PATH];
	if (GetModuleFileNameA(hModule, szModName, MAX_PATH) > 0)
		return szModName;

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::Print() const
{
	EnsureSymInit();

	for (int i = 0; i < frameCount_; ++i)
	{
		char szSymbolBuf[sizeof(SYMBOL_INFO) + 256] = { 0, };
		SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)szSymbolBuf;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = 255;

		DWORD64 dwDisplacement = 0;
		IMAGEHLP_LINE64 stLine = { 0, };
		stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		DWORD dwLineDisplacement = 0;

		BOOL bHasSymbol = SymFromAddr(s_hProcess, (DWORD64)addresses_[i], &dwDisplacement, pSymbol);
		BOOL bHasLine = SymGetLineFromAddr64(s_hProcess, (DWORD64)addresses_[i], &dwLineDisplacement, &stLine);

		const char* szModuleName = GetModuleNameFromAddress(addresses_[i]);

		if (bHasSymbol && bHasLine)
		{
			_LogError_("  #%d: %s (0x%p) [%s:%lu]", i, pSymbol->Name, addresses_[i], stLine.FileName, stLine.LineNumber);
		}
		else if (bHasSymbol)
		{
			if (szModuleName)
				_LogError_("  #%d: %s (0x%p) [%s]", i, pSymbol->Name, addresses_[i], szModuleName);
			else
				_LogError_("  #%d: %s (0x%p)", i, pSymbol->Name, addresses_[i]);
		}
		else
		{
			if (szModuleName)
				_LogError_("  #%d: 0x%p (%s)", i, addresses_[i], szModuleName);
			else
				_LogError_("  #%d: 0x%p", i, addresses_[i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::PrintStack(int _skipFrames)
{
	StackTrace trace;
	trace.Capture(_skipFrames + 1);
	trace.Print();
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::PrintStack(CONTEXT* _pContext)
{
	StackTrace trace;
	trace.Capture(_pContext);
	trace.Print();
}

//////////////////////////////////////////////////////////////////////////////////////////
void StackTrace::WriteMinidump(CONTEXT* _pContext, DWORD _exceptionCode)
{
	char szDumpPath[MAX_PATH] = { 0, };
	char szExePath[MAX_PATH] = { 0, };

	if (GetModuleFileNameA(nullptr, szExePath, MAX_PATH) == 0)
		return;

	char szDrive[_MAX_DRIVE] = { 0, };
	char szDir[_MAX_DIR] = { 0, };
	char szName[_MAX_FNAME] = { 0, };
	_splitpath_s(szExePath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szName, _MAX_FNAME, nullptr, 0);

	time_t now = time(nullptr);
	struct tm local;
	localtime_s(&local, &now);

	sprintf_s(szDumpPath, "%s%s%s_crash_%04d%02d%02d_%02d%02d%02d.dmp",
		szDrive, szDir, szName,
		local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
		local.tm_hour, local.tm_min, local.tm_sec);

	HANDLE hFile = CreateFileA(szDumpPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		_LogError_("미니덤프 파일 생성 실패: %s (GetLastError=%lu)", szDumpPath, GetLastError());
		return;
	}

	MINIDUMP_EXCEPTION_INFORMATION mei = { 0, };
	mei.ThreadId = GetCurrentThreadId();
	mei.ExceptionPointers = nullptr;
	mei.ClientPointers = FALSE;

	EXCEPTION_POINTERS ep;
	EXCEPTION_RECORD er;
	er.ExceptionCode = _exceptionCode;
	er.ExceptionFlags = 0;
	er.ExceptionRecord = nullptr;
#ifdef _M_AMD64
	er.ExceptionAddress = _pContext ? (void*)_pContext->Rip : nullptr;
#else
	er.ExceptionAddress = _pContext ? (void*)_pContext->Eip : nullptr;
#endif
	er.NumberParameters = 0;

	ep.ExceptionRecord = &er;
	ep.ContextRecord = _pContext;
	mei.ExceptionPointers = &ep;

	BOOL bRet = MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		(MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
		_pContext ? &mei : nullptr,
		nullptr,
		nullptr);

	CloseHandle(hFile);

	if (bRet)
		_LogError_("미니덤프 저장 완료: %s", szDumpPath);
	else
		_LogError_("미니덤프 저장 실패: GetLastError=%lu", GetLastError());
}

NS_END
