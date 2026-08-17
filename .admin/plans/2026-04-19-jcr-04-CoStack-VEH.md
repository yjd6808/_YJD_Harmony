# 작업 히스토리: jcr 04_CoStack VEH 핸들러 구현 및 컨벤션 수정

## 날짜
2026-04-19

## 작업 결과
빌드 성공 (경고 0개, 오류 0개)

## 작업 배경
`04_CoStack.cpp`의 `call_04_CoStack` 함수가 동작하지 않는 문제 수정.
StackTouch 시 VEH 핸들러에서 스택을 자동 확장하는 기능 구현.

## 문제점 목록 및 수정 내용

### 기능 오류
| 문제 | 원인 | 수정 |
|------|------|------|
| 컴파일 오류 | `HandleGuardException` 미정의 상태로 `__except` 필터에 사용 | `__try/__except` 블록 제거, 직접 메모리 접근으로 변경 |
| VEH 미동작 | `AddVectoredExceptionHandler` 호출 없이 `VectoredHandler`만 정의 | `call_04_CoStack` 진입 시 등록, 종료 시 `RemoveVectoredExceptionHandler` 해제 |
| 스택 감지 실패 | `RegisterStack` 미호출로 `FindStack`이 항상 `nullptr` 반환 | `StackAlloc` 직후 `RegisterStack`, `StackFree` 직전 `UnregisterStack` 호출 |
| VEH 핸들러 버그 | `next2` 페이지가 `RESERVE` 상태인데 `VirtualProtect(PAGE_GUARD)` 호출 → 실패 | `VirtualAlloc(MEM_COMMIT)` 먼저 수행 후 GUARD 설정 |

### 컨벤션 위반
| 위반 항목 | 수정 |
|-----------|------|
| `dwError` — `dw` 금지 prefix | `error` |
| `vprot` — `v` 금지 prefix | `protResult` |
| 포인터 파라미터 `_p` prefix 누락 (`base`, `addr`, `ep` 등) | `_pBase`, `_pAddr`, `_pEp` 등 |
| 포인터 로컬변수 `p` prefix 누락 (`faultAddr`, `s`, `page`, `next`, `next2` 등) | `pFaultAddr`, `pStack`, `pPage`, `pNext`, `pNext2` 등 |
| `const size_t reserveSize` 지역 상수 | `constexpr size_t RESERVE_SIZE` (UPPER_CASE) |

## 변경된 파일 목록

### 수정
1. `Projects/jcr/Sources/jcr/04_CoStack.cpp`
   - `VectoredHandler` → 파라미터/로컬변수 명명 규칙 수정, `next2` COMMIT 후 GUARD 설정 버그 수정
   - `RegisterStack`, `UnregisterStack`, `FindStack` → 파라미터 명명 규칙 수정
   - `StackAlloc`, `StackInit`, `StackFree`, `DumpPages` → 로컬변수/파라미터 명명 규칙 수정
   - `call_04_CoStack` → `AddVectoredExceptionHandler` + `RegisterStack` 추가, `__try/__except` 제거, `RemoveVectoredExceptionHandler` + `UnregisterStack` 추가
