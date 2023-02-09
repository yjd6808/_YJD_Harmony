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


		// 글로벌 메모리릭 체크
		static JCore::AutoMemoryLeakDetector _2{ [](Int32U uiLeaked) {
			JCore::NormalConsole::WriteLine("[메모리릭 %u바이트]", uiLeaked);
		} };
	}
	NS_DETAIL_END
NS_JC_END
