# 작업: sgf cpp 코딩 컨벤션 준수 수정

> 날짜: 2026-08-09
> 범위: `Projects/sgf/Sources/sgf/**`

## 배경

sgf 프로젝트의 `.cpp` 파일에 함수 시각적 구분자(`/` 90자, 들여쓰기 0 기준) 규칙이 지켜지지 않은 파일들이 확인됨.
또한 `SoundEngine.h`의 중첩 구조체 멤버가 private 멤버 suffix `_` 규칙을 위반.

## 규칙 근거

- `.opencode/rules/coding-conventions_cpp.md` 함수 시각적 구분자 섹션
  - 각 함수 앞에 `/` 문자로 구성된 구분자, 길이 = 90 - (들여쓰기 레벨 × 4)
  - sgf는 `NS_SGF_BEGIN` 네임스페이스로 들여쓰기 레벨 0 → 90자
- private 멤버 변수: suffix `_`

## 발견된 위반

### A. 함수 구분자 누락 (9개 파일)

| 파일 | 누락 함수 수 | 비고 |
|---|---|---|
| Core/Window.cpp | 10 | 전체 |
| Graphics/Buffers.cpp | 10 | 전체, `// ===` 스타일 사용 중 |
| Audio/SoundEngine.cpp | 7 | StopAll, Pause, PauseAll, Resume, ResumeAll, SetVolume, IsPlaying |
| Graphics/RenderTarget.cpp | 7 | 전체 |
| Graphics/Renderer2D.cpp | 7 | 생성자/소멸자/PushQuad/DrawRect/DrawLine/DrawCircle/Flush |
| Graphics/Texture.cpp | 6 | 전체 |
| Input/InputManager.cpp | 4 | 전체 (스캔 기준) |
| Graphics/Shader.cpp | 3 | 전체 |
| Scene/Director.cpp | 2 | 생성자/소멸자 |

### B. 멤버 변수 suffix `_` 누락 (SoundEngine.h, 4건)

- `WavData::format` → `format_`
- `VoiceSlot::audioId` → `audioId_`
- `VoiceSlot::bPaused` → `bPaused_`
- `VoiceSlot::bLoop` → `bLoop_`
- 사용처: SoundEngine.cpp 전반 (참조 갱신 필요)

## 작업 단계

1. 계획 파일 작성 (본 파일)
2. SoundEngine.h 구조체 멤버 rename + SoundEngine.cpp 사용처 갱신
3. 각 파일에 `/` 90자 구분자 삽입
4. 빌드 확인: `Scripts\BuildProject\sgf.bat -C Debug -P x64`
5. 완료 기록 작성

## 완료 조건

- 위 표의 모든 누락 함수 앞에 90자 `/` 구분자 존재
- SoundEngine.h 멤버 4건 suffix `_` 반영 + 사용처 일관성
- 빌드 성공
