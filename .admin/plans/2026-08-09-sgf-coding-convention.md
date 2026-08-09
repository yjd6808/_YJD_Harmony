# sgf 코딩컨벤션 준수 리팩토링 계획 (2026-08-09)

## 목표
sgf 프로젝트 코드를 `.opencode/rules/coding-conventions_cpp.md` 규칙에 맞게 전면 정리

## 승인된 수정 범위 (사용자 확인 완료)
1. **멤버 변수**: `m_` prefix → suffix `_` (예: `m_Width` → `width_`, `m_pDevice` → `pDevice_`)
2. **static const / 상수**: `_v`/`k` prefix → UPPER_SNAKE (예: `MaxQuads_v` → `MAX_QUADS`, `kSampleRate_v` → `SAMPLE_RATE`)
3. **금지 접두사 로컬 변수**: `dwStyle`, `hInstance` → camelCase (`style`, `instance`)
   - ⚠️ 사용자 확인: 단일 `i`/`v` 루프 변수(`for (int i`, `for (int v`)는 **사용 가능** → 변경 안 함
5. **함수 시각적 구분자(/)** 규칙 적용 (.cpp)
6. **가상 함수** `virtual` 명시 확인/보강

## 영향 범위
- sgf 내부 전 파일 (헤더+구현). sgfr은 sgf private 멤버/상수 미접근 확인 → 영향 없음
- 멤버/상수는 sgf 내부에서만 참조되므로 범위 sgf 한정

## 작업 단계
1. [ ] 전체 위반 목록 정밀 수집 (멤버/상수/루프/로컬/구분자/가상함수)
2. [ ] 멤버 변수 m_ → suffix _ 변환 (전 파일)
3. [ ] 상수 UPPER_SNAKE 변환
4. [ ] 금지 접두사 루프/로컬 변수 변환
5. [ ] 함수 구분자 / 가상 함수 정리
6. [ ] 남은 위반 0건 확인
7. [ ] 빌드 검증: jc / sgf / sgfr (Debug x64)
8. [ ] /done 히스토리 작성

## 검증
- sgf 전체 파일에서 `m_` prefix, `_v`/`k` 상수, `for (int i|v`, `dw`/`h` 로컬 0건
- jc / sgf / sgfr Debug x64 빌드 0 error
