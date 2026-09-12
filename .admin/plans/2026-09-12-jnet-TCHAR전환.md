# jnet TCHAR 전환 계획 (2026-09-12)

- 인계 §4-3 두 번째 항목의 첫 단계. jnet Unicode 빌드는 현재 깨져 있음(기준 상태).
- narrow 유지: `TypeName()` 가상군, `_Name()`(Q-11), `CommandNameDictionary`, `ProtocolName`,
  `TransportProtocolName`, `InitializeNetLogger` — 로그 출력 시 `%hs`.

## 1. 분석 결과 (실측, 94파일)

- `_NetLogXXX_("...")` narrow 50곳 — `_T()` 일괄 (assert D-1 sed와 동일 방식, 멀티라인 수동)
- `StringUtilT::Format("...")` narrow 2곳 — `%hs` + `_T()` (`NetLoggerDefine.cpp:23`,
  `WorkerGroup.h:47` `typeid`)
- `IPEndPoint.h:68` `return ""` → `_T("")`
- R8 `IPAddress.h:17` `#ifdef UNICODE` → `#ifdef _UNICODE`
- `IPv4Address`/`IPv4EndPoint` Parse/ctor/`==` narrow → `const _char*`화 4파일
  (`String`판 위임은 그대로, 본문 `_char`화: `IsNumeric` 추론 유지,
  `atoi` → `CRuntime::StrToLong`, `StaticString<4>` 그대로(기본 `_char`),
  `'.'`/`'\0'`/`":"` → `_T()`)
- `ToString()` narrow `ostringstream` → `StringUtilT::Format(_T("%d.%d.%d.%d"))`,
  `+ ':'` → `+ _T(':')`
- `%hs` 검토: `TypeName()`/`_Name()`/`Get()` narrow 인자 로그

## 2. 수정 파일 (예상 20~30개)

`IPAddress.h/.cpp`, `IPEndPoint.h/.cpp`, `NetLoggerDefine.cpp`,
`WorkerGroup.h`, 로그 호출부 약 15파일, `%hs` 대상 파일

## 3. 검증

1. `Scripts\BuildProject\jnet.bat -P x64` → `jnet_gtest.bat -P x64` (Unicode)
2. `Scripts\BuildProject-Multibyte\jnet.bat -P x64` → `jnet_gtest.bat -P x64` (MultiByte)
3. 양쪽 exe 실행: EXIT 0 + 테스트 전수 통과 (건수는 실행 시 확정)
4. Unicode 재빌드로 트리 복구 (jc/jc_gtest는 건드리지 않음)
5. 인계 §4 갱신 + 완료기록 번호 추가.

## 4. 결과 (2026-09-12 11:55)

- 수정 약 25파일 (`_NetLog` 12 + `_Log` 6 + IP 4 + Winsock 3 + gtest 4 + PCH 1)
- Q-5가 미적용 상태였음을 발견하고 실제 적용 (완료기록 9번은 계획 기록이었음)
- 끄져 있던 `jnet_gtest` 4종 활성화 — Parse 재작성 검증됨
- jnet + jnet_gtest Unicode/MultiByte 양쪽 빌드 `[OK]`, 전체 실행 양쪽 EXIT 0,
  **jc 415/415 + jnet 4/4 PASSED**
- Unicode 재빌드로 트리 복구됨 (jc 계열 포함). 완료기록 27번 추가됨.
