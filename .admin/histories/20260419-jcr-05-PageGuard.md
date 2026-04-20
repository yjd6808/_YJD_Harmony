# 작업 히스토리: jcr 05_PageGuard 신규 구현

## 날짜
2026-04-19

## 작업 결과
빌드 성공 (경고 0개, 오류 0개)

## 작업 배경
현재 스레드 스택의 가드 페이지 레이아웃을 탐색하고, 가드 페이지 터치 시 VEH 핸들러가
위반을 감지한 후 OS가 스택을 자동 확장하는 동작을 확인하는 연구 예제 구현.

## 구현 내용

### `PageGuardVectoredHandler`
- 04의 `VectoredHandler`와 역할 분리 — 실제 스레드 스택 전용
- `STATUS_GUARD_PAGE_VIOLATION` 감지 시 fault 주소 로그 출력
- `EXCEPTION_CONTINUE_SEARCH` 반환 → OS 기본 스택 확장 처리에 위임
  (실제 스레드 스택은 OS가 `NT_TIB::StackLimit` 갱신까지 처리하므로 위임이 안전)

### `call_05_PageGuard` 동작 흐름
1. VEH 핸들러 등록 (`AddVectoredExceptionHandler`)
2. 로컬 변수(`tmp`)에서 하위 방향으로 `VirtualQuery` 페이지 순회, GUARD 페이지 수집
3. 루프 탈출 시점(첫 비커밋 페이지) 상태 출력 (하늘색)
4. 가드 페이지 터치 — VEH가 위반 주소 출력 → OS가 스택 확장
5. 터치 후 가드 페이지였던 페이지들의 변화된 상태 출력 (초록색)
6. 루프 탈출 시점 주소 재조회 — OS가 새 가드를 설정한 위치 확인 (하늘색)
7. VEH 해제 (`RemoveVectoredExceptionHandler`)

## 변경된 파일 목록

### 신규 생성
1. `Projects/jcr/Sources/jcr/05_PageGuard.cpp`

### 수정
2. `Projects/jcr/Sources/jcr/Core.h`
   - `call_05_PageGuard` 함수 선언 추가
3. `Projects/jcr/Sources/jcr/main.cpp`
   - `[5] PageGuard - 페이지 가드 터치` 메뉴 항목 및 분기 추가
4. `Projects/jcr/Project/jcr.vcxproj`
   - `05_PageGuard.cpp` ClCompile 항목 추가
