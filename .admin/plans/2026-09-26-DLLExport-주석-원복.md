# DLLExport 주석 원복

## 대상

- `Projects/jc/Sources/jc/Define.h` (JC_DLL_EXPORT 분기 주석)
- `Projects/jc/Sources/jc/DLLExport.h` (SG_DLL_EXPORT 분기 주석)

## 방식

이번 턴 제거분이 두 파일의 유일한 diff임을 확인 후,
HEAD 버전으로 원복한다 (바이트 정확).

## 검증

- 원복 후 Read로 확인.
- `jc.bat -C Debug -P x64` 빌드.
