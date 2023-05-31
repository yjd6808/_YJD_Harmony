/*
 * 작성자: 윤정도
 * 디버그모드일때만 assert 실행
 *
 * assert() 연산 결과값을 사용하지 않는 경우 컴파일 최적화 이뤄지는지 확인
 * https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:50,endLineNumber:9,positionColumn:50,positionLineNumber:9,selectionStartColumn:50,selectionStartLineNumber:9,startColumn:50,startLineNumber:9),source:'//+%EB%A7%A4%ED%81%AC%EB%A1%9C+%EB%82%B4%EC%9A%A9%EC%9D%B4+%EB%B9%84%EC%96%B4%EC%9E%88%EC%9C%BC%EB%A9%B4+%EC%B5%9C%EC%A0%81%ED%99%94+%EC%A0%9C%EC%99%B8%EB%90%98%EB%8A%94%EC%A7%80+%ED%99%95%EC%9D%B8%0A%0A%23include+%3Cmutex%3E%0A%23include+%3Ccassert%3E%0A%0A%23define+DebugMode+0%0A%0A%23if+DebugMode%0A++++%23define+DebugAssertMessage(exp,+msg)%09assert((exp)+%26%26+(msg))%0A++++%23define+DebugAssert(exp)++++++++++++++++assert((exp))%0A%23else%0A++++%23define+DebugAssertMessage(exp,+msg)%09(0)%0A++++%23define+DebugAssert(exp)++++++++++++++++(0)%0A%23endif%0A%0Aint+main()+%7B%0A++++int+rand_val+%3D+rand()%3B%0A++++DebugAssertMessage(rand_val+%3D%3D+rand(),+%22fesfes%22)%3B%0A++++DebugAssert(rand_val+%3D%3D+1)%3B%0A++++return+rand_val%3B%0A%7D'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:45.152274373575885,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:vcpp_v19_latest_x86,deviceViewOpen:'1',filters:(b:'0',binary:'1',commentOnly:'0',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'/O2+',selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86+msvc+v19.latest+(Editor+%231)',t:'0')),k:36.662640612527504,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+gcc+12.2',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86+msvc+v19.latest+(Compiler+%231)',t:'0')),k:18.185085013896632,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4
 */
#pragma once

#include <cstdlib>
#include <JCore/Namespace.h>

// 헤더파일 의존성 회피를 위함
NS_JC_BEGIN
	NS_DETAIL_BEGIN
	void __DebugAssertMsgImpl(const char* filePath, int lineNum, const char* functionName, const char* fmt, ...);
	NS_DETAIL_END
NS_JC_END


#ifndef DebugAssert
    #if DebugMode
		#define DebugAssertMsg(expect, fmt, ...)																\
		do {																									\
			if ((expect)) break;																				\
			JCore::Detail::__DebugAssertMsgImpl(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);			\
		} while (0)
        #define DebugAssert(expect)            DebugAssertMsg(expect, "메시지 없음")


    #else
        #define DebugAssertMsg(expect, ...)	(0)
        #define DebugAssert(expect)         (0)
    #endif
#endif