# 계획: sgfr 코딩컨벤션 준수 수정

- 날짜: 2026-08-15
- 대상: UltimateHarmony 메인 워크트리
- 범위: `Projects\sgfr\Sources\sgfr` 전체 (코어 + 튜토리얼 31개 + Practice) — 사용자 확인 완료(전체 적용)
- 기준: `.opencode/rules/coding-conventions_cpp.md` (AGENTS.md 참조 문서) + sgf 엔진(이미 컨벤션 적용 완료)의 실무 관례
- 제약: jc 등 타 프로젝트 무수정

## 분석 요약 (현재 위반 사항)

| # | 위반 | 대상 |
|---|---|---|
| V1 | 원시 타입 직접 사용 (`int`, `float`, `char`, `double`) | Main.cpp, Registry, 거의 모든 튜토리얼 |
| V2 | `printf` 직접 사용 — v2 규정상 `jc::Console` 사용해야 함 | 튜토리얼 다수 (29는 이미 Console 사용) |
| V3 | .cpp 함수 시각적 구분자(`/` 90자) 누락 | TutorialRegistry.cpp, PracticeRegistry.cpp, TutorialCommon.cpp, 튜토리얼 .cpp 전부 |
| V4 | 구조체 멤버 네이밍: `TutorialEntry::Name_/Fn_` → `name_/fn_` (suffix `_` camelCase) | TutorialRegistry.h/.cpp, PracticeRegistry.cpp, Main.cpp |

## 기준 (ground truth)

- sgf 엔진은 `NS_SGF_BEGIN/END`, 멤버 suffix `_` camelCase (`clearColor_`, `bVsync_`), `_s32` 타입, `jc_assert_msg` 사용.
- 함수 구분자는 코드베이스 관례상 **90자** (`Main.cpp:22` 등 확인 완료). 들여쓰기 레벨 1이면 탭 1개 + 90자.
- `jc::Console::WriteLine(fmt, args)`는 포맷 앞에 개행을 붙이므로(인자 있는 경우), 기존 `printf` 출력과 동일하게 하려면 인자 있는 출력은 `jc::Console::Write(fmt, args)`로 치환한다. (Write는 sprintf 후 출력 — printf와 동일 동작)
- 구조체 공개 멤버는 sgf `Vertex.h`의 `position_`, `color_` 처럼 소문자 + suffix `_`.

## 조치 단계

1. **K1: 코어 파일 수정** — Main.cpp, TutorialRegistry.h/.cpp, PracticeRegistry.h/.cpp, TutorialCommon.h/.cpp
   - 원시 타입 → jc 별칭 (`int`→`_s32`, `char`→`_s8`, `float`→`_f32`, `bool`은 별칭 없음 — jc Type.h에 bool 별칭 없음, 그대로 유지)
   - `printf` → `jc::Console::Write/WriteLine`
   - 함수 구분자 추가, `TutorialEntry` 멤버 `name_`/`fn_`로 rename
2. **K2: 튜토리얼 파일 일괄 수정** — Tutorial/01~31, Practice/01
   - V1/V2/V3 동일 규칙 적용
   - HLSL 셰이더 소스(raw string `R"(...)"`) 내부는 **절대 수정 금지** (HLSL은 float/int가 정상)
3. **K3: 빌드 검증** — `Scripts\BuildProject\sgfr.bat -C Debug -P x64` (성공 기준: 오류 0)
4. **K4: 완료 기록** — `.admin/histories/2026-08-15-sgfr-coding-convention.md`

## 치환 규칙 상세

### V1 타입 치환
- `int` → `_s32` (단, `UINT` 등 Windows/D3D 타입은 유지, HLSL raw string 내부 유지)
- `float` → `_f32`, `double` → `_f64`, `char` → `_s8`
- `bool` → 유지 (jc 별칭 없음)
- `(int)sizeof(...)` → `(_s32)sizeof(...)`

### V2 printf 치환
- `printf("...")` (인자 없음) → `jc::Console::WriteLine("...")` — 단 `\n`이 붙는지 확인: WriteLine이 개행 추가하므로 `printf("...\n")` → `jc::Console::WriteLine("...")`
- `printf("...\n")` → `jc::Console::WriteLine("...")` (WriteLine은 개행 포함 출력)
- `printf("fmt", arg1, ...)` (인자 있음) → `jc::Console::Write("fmt", arg1, ...)` — Write가 printf와 동일 동작 (인자 있는 WriteLine은 앞에 개행을 추가하므로 사용 금지)

### V3 함수 구분자
- 각 함수 정의 앞에 `//` 90개 구분자 + 주석이 있으면 그 위에
- namespace 블록 안(들여쓰기 레벨 1)이면 탭 1개 + 90자, 레벨 0이면 90자

### V4 구조체 멤버
- `TutorialEntry::Name_` → `name_`, `Fn_` → `fn_` (사용처 전부 갱신)

## 확인 필요
- (완료) 사용자: 전체 적용 확정
- HLSL raw string 내부 치환 방지 — 각 파일 수정 시 raw string 블록 `R"(` ~ `)"` 구간은 스킵 필수
