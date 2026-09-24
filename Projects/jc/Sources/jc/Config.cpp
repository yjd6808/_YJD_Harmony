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

#include <locale.h>

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
	case STATUS_GUARD_PAGE_VIOLATION:
		return false;
	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
struct DumpThreadArgs
{
	PCONTEXT	pContext;
	DWORD		code;
};

static DWORD WINAPI DumpThreadProc(void* _pParam)
{
	DumpThreadArgs* pArgs = (DumpThreadArgs*)_pParam;
	StackTrace::WriteMinidump(pArgs->pContext, pArgs->code);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
static LONG CALLBACK VectoredExceptionHandler(PEXCEPTION_POINTERS _pExceptionInfo)
{
	PEXCEPTION_RECORD pRecord = _pExceptionInfo->ExceptionRecord;

	if (!IsFatalException(pRecord->ExceptionCode))
		return EXCEPTION_CONTINUE_SEARCH;

	_LogError_(_T("========================================"));
	_LogError_(_T("  치명적 예외 발생!"));
	_LogError_(_T("  예외 코드 : 0x%08X"), pRecord->ExceptionCode);
	_LogError_(_T("  예외 주소 : 0x%p"), pRecord->ExceptionAddress);

	if (pRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && pRecord->NumberParameters >= 2)
	{
		_LogError_(_T("  위반 주소 : 0x%p"), (void*)pRecord->ExceptionInformation[1]);
	}

	_LogError_(_T("  예외 시점 콜스택 (Context 기반):"));
	StackTrace::PrintStack(_pExceptionInfo->ContextRecord);

	_LogError_(_T("  현재 스레드 콜스택 (CaptureStackBackTrace 기반):"));
	StackTrace::PrintStack(3);

	_LogError_(_T("========================================"));

	// 덤프는 별도 스레드에서 쓴다.
	// - 코루틴 오버플로우처럼 폴트 스택에 여유가 없을 때 같은 스택에서 쓰면
	//   이중 폴트로 죽는다. 실패하면 기존 방식(같은 스택)으로 폴백한다.
	DumpThreadArgs dumpArgs{ _pExceptionInfo->ContextRecord, pRecord->ExceptionCode };
	HANDLE hDumpThread = ::CreateThread(nullptr, 64 * 1024, DumpThreadProc, &dumpArgs, 0, nullptr);
	if (hDumpThread != nullptr)
	{
		::WaitForSingleObject(hDumpThread, 30000);
		::CloseHandle(hDumpThread);
	}
	else
	{
		StackTrace::WriteMinidump(_pExceptionInfo->ContextRecord, pRecord->ExceptionCode);
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

#ifdef _DEBUG
	::TerminateProcess(GetCurrentProcess(), pRecord->ExceptionCode);
#endif

	return EXCEPTION_CONTINUE_SEARCH;
}

void InitializeJCore(int _argc, char** _argv)
{
	// -----------------------------------------------------------------------
	// 콘솔 출력 파이프라인 (번호 = 발생 순서, 막 = 주체·시점 경계, 예: 한 U+D55C)
	//  막이 바뀌면 "누가·언제·무엇을 고정하는가"가 바뀐다. 깨지면 번호로 막을 특정한다.
	//
	//  0막 디스크 (행위자: 에디터, 시점: 컴파일 전)
	//   1. 소스를 UTF-8로 저장한다. (한 = ED 95 9C 3바이트)
	//      어긋나면 컴파일은 되지만 리터럴이 깨진다. (경고 C4819, 현재 비활성)
	//
	//  1막 컴파일 (행위자: 컴파일러, 시점: 빌드)
	//   2. /utf-8 옵션으로 소스를 UTF-8로 읽어 코드포인트(U+D55C)로 해독한다.
	//   3. Unicode 문자셋 설정으로 _T→L, _tprintf→wprintf, String→WString이 확정된다. (매크로 선택)
	//   4. wide 리터럴은 exe .rdata에 UTF-16LE(5C D5 2바이트)로 주조된다. (메모리 wchar_t = 0xD55C)
	//      narrow 리터럴은 /utf-8 실행문자집합을 따라 UTF-8 바이트 그대로 기록된다.
	//      여기까지는 콘솔·로케일과 무관하게 항상 정상이다.
	//
	//  2막 부팅 (행위자: OS·UCRT·아래 초기화 코드, 시점: 실행 시작 1회)
	//   5. OS가 exe를 로드하고 .rdata를 매핑한다.
	//   6. UCRT 시작 코드가 로케일을 C(ASCII만)로 초기화하고 stdout을 콘솔 핸들에 연결한다.
	//   7. 정적 초기화가 실행된다.
	//   아래 8.~11.을 순서대로 수행한다.
	//
	//  3막 호출 (행위자: Console::Write·콘솔 호스트, 시점: 출력할 때마다)
	//   12. _stprintf_s가 wide→wide로 메모리를 조립한다. (로케일 무관)
	//   13. wprintf가 로케일 변환표로 wchar를 바이트화한 뒤 WriteFile로 전달한다.
	//        로케일 C면 변환 불가 → '?' 치환 또는 실패(-1). 옛날 ????의 발생 지점.
	//   14. 콘솔 호스트가 OutputCP 대조표로 바이트를 해석·렌더링한다.
	//        UTF-8 바이트 + CP 949면 외계어. WriteConsoleW는 13·14번을 건너뛰고 wchar를 직행한다.
	bool bRet = true;
	bRet = Console::Init(); // 8. 출력 핸들 캐시
	jc_assert_msg(bRet, _T("콘솔 출력 스트림 핸들 얻기 실패"));

	// 9. 13번 변환용 UTF-8 변환표 장착
	::setlocale(LC_ALL, ".UTF8");

	// 10. 14번 해석용 출력 CP를 UTF-8로 통일
	bRet = Console::SetOutputCodePage(UTF8);
	jc_assert_msg(bRet, _T("콘솔 출력 스트림 UTF8 코드페이지 변경 실패"));

	// 11. VT 이스케이프 허용
	Console::SetEnableVTMode(true);
	// -----------------------------------------------------------------------

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
