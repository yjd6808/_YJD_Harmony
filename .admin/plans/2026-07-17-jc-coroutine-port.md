# jc 코루틴 포팅 계획

## 목적
jcr 프로젝트에 구현된 코루틴 기능(CoRun/CoYield/CoResume, CoMgr, CoVEH)을 
jc 정적 라이브러리 프로젝트로 이식하여 모든 프로젝트에서 사용할 수 있게 함.

## 작업 내용
1. `Sources/jc/Coroutine/Coroutine.h` - 헤더 파일 생성
2. `Sources/jc/Coroutine/Coroutine.cpp` - C++ 구현 파일 생성
3. `Sources/jc/Coroutine/Coroutine.asm` - ASM 구현 파일 생성
4. `Project/jc.vcxproj` - 새 파일 추가 및 MASM 빌드 지원
5. `Project/jc.vcxproj.filters` - 필터 추가
6. `Sources/jc/PCH/Core.h` - Coroutine.h include 추가

## 출처
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h` → 헤더
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp` → C++ 구현
- `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm` → ASM 구현