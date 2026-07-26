/*
 * 작성자: 윤정도
 * 생성일: 10/27/2023 1:05:36 PM
 * =====================
 *
 */

#include "jc/Config.h"
#include "jc/Time.h"
#include "jc/Container/PropertyStatics.h"
#include "jc/Env.h"
#include "jc/Utils/StackTrace.h"

#include "timeapi.h"

NS_JC_BEGIN

bool AppExited = false;
_u32 AppStartUpTimeTGT_v = ::timeGetTime();
_s64 AppStartUpTime_v = DateTime::Now().Tick;

//////////////////////////////////////////////////////////////////////////////////////////
static bool IsFatalException(DWORD _code)
{
	switch (_code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_STACK_OVERFLOW:
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_PRIV_INSTRUCTION:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	case EXCEPTION_BREAKPOINT:
		return true;
	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
static LONG CALLBACK VectoredExceptionHandler(PEXCEPTION_POINTERS _pExceptionInfo)
{
	PEXCEPTION_RECORD pRecord = _pExceptionInfo->ExceptionRecord;

	if (!IsFatalException(pRecord->ExceptionCode))
		return EXCEPTION_CONTINUE_SEARCH;

	_LogError_("========================================");
	_LogError_("  치명적 예외 발생!");
	_LogError_("  예외 코드 : 0x%08X", pRecord->ExceptionCode);
	_LogError_("  예외 주소 : 0x%p", pRecord->ExceptionAddress);

	if (pRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && pRecord->NumberParameters >= 2)
	{
		_LogError_("  위반 주소 : 0x%p", (void*)pRecord->ExceptionInformation[1]);
	}

	_LogError_("  예외 시점 콜스택 (Context 기반):");
	StackTrace::PrintStack(_pExceptionInfo->ContextRecord);

	_LogError_("  현재 스레드 콜스택 (CaptureStackBackTrace 기반):");
	StackTrace::PrintStack(3);

	_LogError_("========================================");

	StackTrace::WriteMinidump(_pExceptionInfo->ContextRecord, pRecord->ExceptionCode);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

#ifdef _DEBUG
	::TerminateProcess(GetCurrentProcess(), pRecord->ExceptionCode);
#endif

	return EXCEPTION_CONTINUE_SEARCH;
}

void InitializeJCore(int _argc, char** _argv)
{
	// 콘솔출력 인코딩 설정
	//  - 전 소스파일 UTF8 통일
	//  - MSVC 컴파일러 옵션 설정도 변경함
	bool bRet = true;
	bRet = Console::Init();
	jc_assert_msg(bRet, "콘솔 출력 스트림 핸들 얻기 실패");

	bRet = Console::SetOutputCodePage(UTF8);
	jc_assert_msg(bRet, "콘솔 출력 스트림 UTF8 코드페이지 변경 실패");

	Console::SetEnableVTMode(true);

	// VEH 등록 전 DbgHelp 강제 초기화 (PDB 로드 보장)
	StackTrace::EnsureSymInit();

	// 치명적 예외 발생시 스택트레이스 출력을 위한 VEH 등록
	::AddVectoredExceptionHandler(1, VectoredExceptionHandler);

	// 종료시 메모리릭 덤프 출력
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

	// 프로퍼티 초기화
	Env::InitArgs(_argc, _argv);
	Env::CurrentDirectory(); // 캐싱
	Env::ModulePath();	// 캐싱
	Env::ModuleFileName();	// 캐싱
	Env::ModuleName();	// 캐싱
	PropertyStatics::Initialize();
}

void FinalizeJCore()
{
	PropertyStatics::Finalize();
	AppExited = true;
}


NS_END
