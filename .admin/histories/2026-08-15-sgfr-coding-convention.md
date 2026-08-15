# Done: sgfr 프로젝트 코딩 컨벤션 적용 (전체)

## 날짜
2026-08-15

## 작업 내용

`Projects/sgfr` 전체 소스(코어 + 튜토리얼 31개 + Practice)에 `coding-conventions_cpp.md` 적용. 총 64개 파일 수정.

### K1. 코어 파일 (7개)
- **`sgfr/TutorialRegistry.h/.cpp`, `sgfr/Practice/PracticeRegistry.h/.cpp`**
  - `TutorialEntry` 멤버 `Name_/Fn_` → `name_/fn_` (private 멤버 suffix `_` 규칙)
  - `int` → `_s32` (`TutorialCount`, `TutorialAt`, `PracticeCount`, `PracticeAt`, `sizeof` 캐스트)
  - .cpp에 함수 구분자(/ 90자) 추가
- **`sgfr/Main.cpp`**
  - `_s32`/`_s8` 타입 적용 (`ReadSelection`, `szLine[64]`, `main(_s32 _argc, _s8** _argv)` → `_s32 main`)
  - `entry.name_/fn_` 사용처 갱신
- **`sgfr/Common/TutorialCommon.cpp/.h`**
  - 구분자 교체(`//====` → `/` 90자) + `for (int i)` → `_s32`
  - **K1 편집 실수로 생긴 중복 라인 `return R"(`(2행) 제거 — raw string 내부 셰이더 소스 오염 버그 수정**

### K2. 튜토리얼 31개 + Practice (57개 파일)
규칙 적용 (서브에이전트 4개 병렬 + 직접 처리 3개):
1. **타입 별칭 치환**: `int`→`_s32`, `float`→`_f32`, `(int)캐스트`→`(_s32)`, `char 버퍼[N]`→`_s8[N]`
   - **유지**: `const char*`(sgf 엔진 ground truth), `wchar_t`, `bool`, `UINT`/D3D/엔진 타입, `%d`/`%f` 포맷 지정자
   - **HLSL raw string `R"(...)"` 내부는 전부 보호** (float4/float3/int gMode 등 그대로)
2. **printf → jc::Console** (전부 `jc::Console::` 정규화)
   - 인자 있음 → `jc::Console::Write("fmt", args...)` (내부 \n 유지)
   - 인자 없음 + `\n` 종료 → `jc::Console::WriteLine("...")` (trailing \n 제거, `\n\n`은 안쪽 1개 유지)
   - 인자 없음 + \n 없음 → `jc::Console::Write("...")`
   - 인자 없는 문자열의 `%%` → `%` (Write/WriteLine no-arg는 printf 포맷 처리를 안 하므로)
3. **함수 구분자**: 함수 2개 이상인 .cpp의 각 함수 위에 `/` 90자 (레벨 1은 탭 1개 + 90자). .h/단일함수 파일 제외

### K5. 29번 레거시 네이밍 정리 (검토 중 발견, 29_SceneGraph_Main.cpp)
- `m_` 헝가리안 멤버 11개 → suffix `_` 스타일: `sunTexture_`, `earthTexture_`, `moonTexture_`, `elapsed_`, `diagFrames_`, `textures_`, `positionsX_/Y_`, `velocitiesX_/Y_`, `radii_`
- Cocos2d 레거시 `k`-접두사 상수 4개 → UPPER_SNAKE: `kViewWidth_v`→`VIEW_WIDTH`, `kViewHeight_v`→`VIEW_HEIGHT`, `kSegments_v`→`SEGMENTS`, `kBallCount_v`→`BALL_COUNT`
- 전 튜토리얼 재스캔 결과 `m_`/`k*_v` 패턴은 29번이 유일 (g_cDevice 등 엔진 전역은 유지)

## 검증
- `Scripts\BuildProject\sgfr.bat -C Debug -P x64` 빌드 성공 (jc → sgf → sgfr 전 체인 OK, 로그 sgs-20260815-152721.log, 재빌드 sgs-20260815-153008.log)
- 잔여 코드토큰 스캔 (raw string/주석 제거 후 정규식 검사): `printf`(fprintf/vfprintf 제외), `int`, `float` 코드 토큰 0건 — 문자열 리터럴 내부 표기만 잔존(의도)
- 함수 구분자 커버리지: 함수 2개 이상 .cpp 전부 구분자 존재 확인

## 비고
- `fprintf`/`vfprintf`(16, 29 튜토리얼 — 파일 출력)는 콘솔 변환 범위 밖이라 유지
- `wchar_t szTitle[256]`, `const wchar_t*` + `L"..."` 배열은 sgf/jc 관례 따라 유지
- LF로 저장된 파일 12개를 바이트 단위로 CRLF 정규화 (인코딩 보존)
- K2D 서브에이전트가 30/31 파일을 처리하지 못해 수동으로 직접 수정 완료
