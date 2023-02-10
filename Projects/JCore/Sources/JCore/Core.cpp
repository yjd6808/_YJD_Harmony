#include <JCore/Core.h>
#include <JCore/Time.h>

NS_JC_BEGIN


Int64 AppTime_v = DateTime::Now().GetTick();


	NS_DETAIL_BEGIN
	void __JCoreInitialize() {
		// 콘솔출력 인코딩 설정
		//  - 전 소스파일 UTF8 통일
		//  - MSVC 컴파일러 옵션 설정도 변경함
		JCore::NormalConsole::Init();
		JCore::NormalConsole::SetOutputCodePage(JCore::UTF8);

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	}
	NS_DETAIL_END
NS_JC_END
