# jc 코루틴 포팅 완료

## 작업 내용
jcr 프로젝트의 코루틴 기능(CoRun/CoYield/CoResume, CoMgr, CoVEH)을 
jc 정적 라이브러리로 이식 완료.

## 변경된 파일

### 생성된 파일
- `Projects/jc/Sources/jc/Threading/Coroutine.h` - 코루틴 헤더
- `Projects/jc/Sources/jc/Threading/Coroutine.cpp` - C++ 구현
- `Projects/jc/Sources/jc/Threading/Coroutine_Asm.asm` - ASM 구현

### 수정된 파일
- `Projects/jc/Project/jc.vcxproj` - 새 파일 추가 및 MASM 빌드 지원
- `Projects/jc/Project/jc.vcxproj.filters` - 필터 추가
- `Projects/jc/Sources/jc/PCH/Core.h` - Coroutine.h include 추가

## 출처
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h` → `Coroutine.h`
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp` → `Coroutine.cpp`  
- `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm` → `Coroutine_Asm.asm`

## 빌드 결과
- Debug x64: 성공 (경고 0, 오류 0)