#include <JCore/Core.h>
#include <JCore/Time.h>

NS_JC_BEGIN


Int64 AppTime_v = DateTime::Now().GetTick();


	NS_DETAIL_BEGIN
	void InitializeJCore() {
		// 콘솔출력 인코딩 설정
		//  - 전 소스파일 UTF8 통일
		//  - MSVC 컴파일러 옵션 설정도 변경함
		bool bRet = true;

		bRet = JCore::Console::Init();
		DebugAssertMsg(bRet, "콘솔 출력 스트림 핸들 얻기 실패");

		bRet = JCore::Console::SetOutputCodePage(UTF8);
		DebugAssertMsg(bRet, "콘솔 출력 스트림 UTF8 코드페이지 변경 실패");

		bRet = JCore::Console::SetEnableVTMode(true);
		DebugAssertMsg(bRet, "콘솔 버철 터미널 모드 변경 실패");

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	}
	NS_DETAIL_END
NS_JC_END
