/*
 * 작성자: 윤정도
 * 생성일: 12/5/2022 8:45:26 PM
 * =====================
 * 아무 기능을 하지 않는 락
 * 컴파일 최적화시 코드에 포함되지 않는다는 점을
 * 이용해서 락이 필요한 상황, 없는 상황을
 * 컴파일 타임에 효과적으로 처리하기 위함
 * 아래 Compiler Explorer 테스트 결과를 보면 최적화 수행시
 * 코드에 포함되지 않는 것을 확인할 수 있다.
 * 코드가 복잡해져도 포함안되겠지?
 * https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:21,endLineNumber:27,positionColumn:21,positionLineNumber:27,selectionStartColumn:21,selectionStartLineNumber:27,startColumn:21,startLineNumber:27),source:'//+%EB%B9%88+%ED%95%A8%EC%88%98+%EC%B5%9C%EC%A0%81%ED%99%94+%EC%9D%B4%EB%A4%84%EC%A7%80%EB%8A%94%EC%A7%80+%EC%97%AC%EB%B6%80%0A%0A%23include+%3Cmutex%3E%0A%0Ausing+namespace+std%3B%0A%0Astruct+ilock+%7B%0A++++virtual+void+lock()+%3D+0%3B%0A++++virtual+void+unlock()+%3D+0%3B%0A%7D%3B%0A%0Astruct+unused_lock+:+ilock+%7B%0A++++void+lock()+override+%7B%7D%0A++++void+unlock()+override+%7B%7D%0A%7D%3B%0A%0Atemplate%3Ctypename+TLock%3E%0Aclass+Base+%7B%0Apublic:%0A++++int+write_data(int+a)+%7B%0A++++++++m_lock.lock()%3B%0A++++++++int+result+%3D+m_value+%3D+a%3B%0A++++++++m_lock.unlock()%3B%0A++++++++return+result%3B%0A++++%7D%0A%0A++++int+m_value+%3D+0%3B%0A++++TLock+m_lock%3B%0A%7D%3B%0A%0A%0Aint+main()+%7B%0A++++Base%3Cunused_lock%3E+base2%3B%0A++++return+base2.write_data(rand())%3B%0A%7D'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:34.37009110901666,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:g122,deviceViewOpen:'1',filters:(b:'0',binary:'1',commentOnly:'0',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B23+-m32+-O2',selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+gcc+12.2+(Editor+%231)',t:'0')),k:32.29657555765002,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+gcc+12.2',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+gcc+12.2+(Compiler+%231)',t:'0')),k:33.33333333333333,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4
 *
 */


#pragma once

#include <JCore/Sync/ILock.h>

NS_JC_BEGIN

class UnusedLock final : public ILock
{
public:
void Lock() override {}
bool TryLock() override { return true; }
void Unlock() override {}
bool IsLocked() override { return false; }
};

using UnusedLockGuard = LockGuard<UnusedLock>;
extern template         LockGuard<UnusedLock>;

NS_JC_END;

