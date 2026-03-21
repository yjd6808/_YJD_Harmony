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

#include "timeapi.h"

NS_JC_BEGIN

bool AppExited = false;
_u32 AppStartUpTimeTGT_v = ::timeGetTime();
_s64 AppStartUpTime_v = DateTime::Now().Tick;

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

	bRet = Console::SetEnableVTMode(true);
	jc_assert_msg(bRet, "콘솔 버철 터미널 모드 변경 실패");

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
