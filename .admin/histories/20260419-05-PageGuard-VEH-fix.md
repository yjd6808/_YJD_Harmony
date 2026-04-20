# 작업 히스토리: 05_PageGuard VEH 연속 호출 수정

## 날짜
2026-04-19

## 작업 결과
빌드 성공 (경고 0개, 오류 0개)

## 작업 배경
`PageGuardVectoredHandler`에서 `EXCEPTION_CONTINUE_SEARCH`를 반환할 때,
OS가 새로 설정한 가드 페이지를 터치해도 VEH가 호출되지 않는 문제.

## 원인
`EXCEPTION_CONTINUE_SEARCH` 반환 시 OS 내부 핸들러(ntdll)가 스택 확장을 커널 수준에서
직접 처리한다. 이 경우 이후 OS가 설정한 가드 페이지 위반은 커널이 직접 처리하여
user-mode VEH 체인이 다시 호출되지 않는다.

## 수정 내용
`EXCEPTION_CONTINUE_SEARCH` → `EXCEPTION_CONTINUE_EXECUTION`으로 변경하고,
스택 확장을 VEH 내에서 직접 처리:

1. **스택 범위 검사**: `NtCurrentTeb()`으로 `NT_TIB::StackBase` / `StackLimit` 조회,
   범위 외 가드 페이지는 `EXCEPTION_CONTINUE_SEARCH`로 기존 위임 유지
2. **가드 페이지 해제**: `VirtualProtect(pPage, PAGE_READWRITE)`
3. **다음 페이지 확장**: `VirtualAlloc(pNext, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD)`
4. **`NT_TIB::StackLimit` 갱신**: 런타임(`_chkstk` 등)이 새 스택 하한선을 인식하도록 업데이트
5. **`EXCEPTION_CONTINUE_EXECUTION` 반환**: OS가 개입하지 않으므로 이후 가드 페이지도 VEH 경유

## 변경된 파일 목록

### 수정
1. `Projects/jcr/Sources/jcr/05_PageGuard.cpp`
   - `PageGuardVectoredHandler` 전면 재작성
