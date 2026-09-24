/*
 * 작성자 : 윤정도
 * TryFindContextById 제거 plan
 */

# TryFindContextById 제거

## 배경
- `FindContextById`가 이미 nullptr를 돌려주므로 `Try` 래퍼가 중복이다.
- `TryFindContextById` 호출 3곳은 모두 `!Try(...) || pCtx == nullptr` 패턴이라 `Find` + nullptr 검사로 그대로 전환된다.

## 범위 (2파일)
- `Projects/jc/Sources/jc/Threading/Coroutine.h`: 선언 1행 삭제
- `Projects/jc/Sources/jc/Threading/Coroutine.cpp`
  - 정의 삭제 + 구역 주석 수정
  - `CoLookupResumable`, `CoScope::~CoScope`, `CoScope::Cancel` 3곳 전환

## 제외 (별도 보고)
- `TryFindContextByBase`, `TryFindStackByBase/Addr`는 호출처 0건(dead)이지만 이번 범위 밖.
- `TryFindContextByAddr`는 `CoVEH`에서 1회 사용 중이라 유지.

## 검증
- `Scripts\BuildProject\jc_gtest.bat -C Debug -P x64`
