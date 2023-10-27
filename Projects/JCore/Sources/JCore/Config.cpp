/*
 * 작성자: 윤정도
 * 생성일: 10/27/2023 1:05:36 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Config.h>
#include <JCore/Time.h>
#include <JCore/Container/PropertyStatics.h>

NS_JC_BEGIN

bool AppExited = false;
Int64 AppStartUpTime_v = DateTime::Now().Tick;

void InitializeJCore() {
	// 콘솔출력 인코딩 설정
	//  - 전 소스파일 UTF8 통일
	//  - MSVC 컴파일러 옵션 설정도 변경함
	bool bRet = true;
	bRet = Console::Init();
	DebugAssertMsg(bRet, "콘솔 출력 스트림 핸들 얻기 실패");

	bRet = Console::SetOutputCodePage(UTF8);
	DebugAssertMsg(bRet, "콘솔 출력 스트림 UTF8 코드페이지 변경 실패");

	bRet = Console::SetEnableVTMode(true);
	DebugAssertMsg(bRet, "콘솔 버철 터미널 모드 변경 실패");

	// 종료시 메모리릭 덤프 출력
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

	// 프로퍼티 초기화
	PropertyStatics::Initialize();
}

void FinalizeJCore() {
	PropertyStatics::Finalize();
	AppExited = true;
}


NS_JC_END

