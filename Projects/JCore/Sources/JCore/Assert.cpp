/*
 *	작성자 : 윤정도
 *	작성일 : 2023/02/05(일요일 오후 3시 35분 경)
 */


#include <JCore/Core.h>
#include <JCore/Assert.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/FileSystem/Path.h>

NS_JC_BEGIN
	NS_DETAIL_BEGIN
	void __DebugAssertMsgImpl(const char* _expectStr, const char* _filePath, int _lineNum, const char* _functionName, const char* _fmt, ...) {

		// 내부에서 오류 발생시 DebugAssert 재귀 호출되기땜에 여기선 오류 절대 발생하면안댐
		static bool PreventUnrechableCode = true;
		static constexpr int BufSize = 512;													
		char szFmtBuf[BufSize] = { 0, };													
		if (StringUtil::Length(_fmt) > 0) {
			va_list args;
			va_start(args, _fmt);
			StringUtil::FormatBuffer(szFmtBuf, BufSize, _fmt, args);
			va_end(args);
		}
		StringUtil::ConcatInnerFront(szFmtBuf, BufSize, "┌ 어썰트 발생 : ");
		printf("%s\n", szFmtBuf);															
		Path::FileNameLevel(szFmtBuf, BufSize, _filePath, StringUtil::Length(_filePath), 2);
		printf("│ EXP : %s\n", _expectStr);
		printf("│ 파일 : %s\n", szFmtBuf);												    
		printf("│ 라인 : %d\n", _lineNum);
		printf("└ 함수 : %s\n", _functionName);
		if (PreventUnrechableCode) std::abort();

	}
	NS_DETAIL_END

NS_JC_END

