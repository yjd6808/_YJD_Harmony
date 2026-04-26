# History: 04-CoStack 파일명변경 + 06-Coroutine 스켈레톤 생성

**날짜:** 2026-04-25  
**작업명:** 04-CoStack-파일명변경-06-Coroutine-스켈레톤생성  
**상태:** 완료

## 작업 요약

`04_CoStack.cpp/.asm`을 파일명 기반으로 분리하고, 코루틴 전용 파일(`06_Coroutine*.cpp/.asm`)을 신규 생성하였다.  
plan 내용보다 실제 구현이 더 세분화되어, 구현부 파일이 별도 `06_CoroutineImpl1.cpp/.h` / `06_CoroutineImpl2.asm`으로 나뉘었다.

## 수행된 변경 사항

### 신규 생성 파일
- `Projects/jcr/Sources/jcr/04_CoStack1.cpp` — 기존 `04_CoStack.cpp` 대체 (파일명 변경)
- `Projects/jcr/Sources/jcr/04_CoStack2.asm` — CoStack 전용 ASM (co_run 제거됨)
- `Projects/jcr/Sources/jcr/06_Coroutine.cpp` — call_06_Coroutine 진입 함수
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.cpp` — CoroutineMgr 등 코루틴 구현 코드
- `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h` — 코루틴 구현 헤더
- `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm` — co_run proc 이동

### 수정된 파일
- `Projects/jcr/Sources/jcr/04_CoStack.cpp` — 삭제 (04_CoStack1.cpp로 대체)
- `Projects/jcr/Sources/jcr/Core.h` — `call_06_Coroutine` 선언 추가
- `Projects/jcr/Sources/jcr/main.cpp` — 6번 메뉴 항목 추가 ("Coroutine - 코루틴 연구")
- `Projects/jcr/Project/jcr.vcxproj` — 신규 파일 등록 (ClCompile, ClInclude, MASM)
- `Projects/jc/Project/jc.vcxproj.filters` — 필터 업데이트
- `Projects/jcr/Sources/jcr/01_AsmStudy1.cpp` — 관련 수정
- `Projects/jcr/Sources/jcr/01_AsmStudy2.asm` — 관련 수정

## Plan과의 차이점

| 항목 | Plan | 실제 구현 |
|------|------|-----------|
| 06 Coroutine 파일 구성 | `06_Coroutine1.cpp`, `06_Coroutine2.asm` | `06_Coroutine.cpp`, `06_CoroutineImpl1.cpp`, `06_CoroutineImpl1.h`, `06_CoroutineImpl2.asm` |
| 04_CoStack ASM | `04_CoStack.asm` 정리 | `04_CoStack2.asm`으로 파일명 변경 |

## 참고

- Plan 파일: `.admin/plans/2026-04-25-04-CoStack-파일명변경-06-Coroutine-스켈레톤생성.md`
- 연관 plan: `.admin/plans/2026-04-25-06-Coroutine-to-04-CoStack-통합.md`
