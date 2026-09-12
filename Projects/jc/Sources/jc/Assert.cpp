/*
 *	작성자 : 윤정도
 *	작성일 : 2023/02/05(일요일 오후 3시 35분 경)
 */


#include "jc/Assert.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/IO/Path.h"
#include "jc/Logger/LoggerDefine.h"
#include "jc/Utils/StackTrace.h"

NS_JC_BEGIN
	NS_DETAIL_BEGIN

	/////////////////////////////////////////////////////////////////////////////////
	void __DebugAssertMsgImpl(const char* _expectStr, const _char* _filePath, int _lineNum, const _char* _functionName, const _char* _fmt, ...) 
	{

		// 내부에서 오류 발생시 DebugAssert 재귀 호출되기땜에 여기선 오류 절대 발생하면안댐
		static bool PreventUnrechableCode = true;
		static constexpr int BufSize = 512;													
		_char szFmtBuf[BufSize] = { 0, };													
		if (StringUtil::Length(_fmt) > 0) {
			va_list args;
			va_start(args, _fmt);
			StringUtil::FormatBuffer(szFmtBuf, BufSize, _fmt, args);
			va_end(args);
		}
		StringUtil::ConcatInnerFront(szFmtBuf, BufSize, _T("┌ 어썰트 발생 : "));
		_tprintf(_T("%s\n"), szFmtBuf);															
		_char szFileBuf[BufSize] = { 0, };
		Path::FileNameLevel(szFileBuf, BufSize, _filePath, StringUtil::Length(_filePath), 2);
		_tprintf(_T("│ EXP : %hs\n"), _expectStr);
		_tprintf(_T("│ 파일 : %s\n"), szFileBuf);												    
		_tprintf(_T("│ 라인 : %d\n"), _lineNum);
		_tprintf(_T("└ 함수 : %s\n"), _functionName);

		StackTrace::PrintStack(1);

		if (PreventUnrechableCode) std::abort();

	}
	NS_END

NS_END

