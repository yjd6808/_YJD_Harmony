# 작업 완료: sgf cpp 코딩 컨벤션 준수 수정

> 날짜: 2026-08-09
> 계획: `.admin/plans/2026-08-09-sgf-cpp-coding-convention.md`

## 수행 내용

### A. 함수 시각적 구분자 (`/` 90자) 추가 — 9개 파일

규칙: 각 함수 앞 `/` 문자 구분자, 길이 = 90 - (들여쓰기 레벨 × 4). sgf는 `NS_SGF_BEGIN` 레벨 0 → 90자.

| 파일 | 함수 수 |
|---|---|
| Core/Window.cpp | 10 |
| Graphics/Buffers.cpp | 10 |
| Audio/SoundEngine.cpp | 7 |
| Graphics/RenderTarget.cpp | 8 |
| Graphics/Renderer2D.cpp | 7 |
| Graphics/Texture.cpp | 6 |
| Input/InputManager.cpp | 10 |
| Graphics/Shader.cpp | 5 |
| Scene/Director.cpp | 2 |
| **합계** | **65** |

### B. 멤버 변수 suffix `_` 규칙 — SoundEngine.h

`WavData`/`VoiceSlot` 구조체 멤버 4건 rename + `SoundEngine.cpp` 사용처 전부 갱신:
- `format` → `format_`
- `audioId` → `audioId_`
- `bPaused` → `bPaused_`
- `bLoop` → `bLoop_`

### C. 문서 정리

- 바탕화면 `코딩컨벤션_C++.md` 작성
- `.opencode/rules/coding-conventions_cpp.md` enum 멤버 네이밍 규칙 추가 (enum class PascalCase / 일반 enum `bl` 접두사)

## 검증

- 빌드: `Scripts\Build.ps1 -ProjectName sgf -Configuration Debug -Platform x64` → 성공 (경고 0, 오류 0)
- 재스캔으로 함수 구분자 누락 0건 확인

## 참고

- `BatchRenderer.cpp`, `GraphicDevice.cpp`, `RenderStates.cpp`, `Camera.cpp`는 원래 규칙 준수 상태로 변경 없음
