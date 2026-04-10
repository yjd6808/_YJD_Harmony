# settings.local.json 설명

## Permissions

### Allow (자동 허용)
사용자 확인 없이 자동으로 실행되는 도구 목록입니다.

### Ask (확인 필요)

## Hooks

### Stop — `complete.wav`
Claude가 모든 도구 실행을 마치고 응답을 완료했을 때 재생됩니다.
- 이벤트: `Stop`
- 파일: `.claude/sound/complete.wav`

### Notification — `sparkle.wav`
Claude가 사용자 확인이 필요한 도구를 실행하려 할 때 재생됩니다.
- 이벤트: `Notification`
- 파일: `.claude/sound/sparkle.wav`

## Sound 파일 위치
`.claude/sound/` 디렉토리에 WAV 파일을 직접 배치해야 합니다.

| 파일 | 용도 |
|---|---|
| `complete.wav` | 작업 완료 알림 |
| `sparkle.wav` | 사용자 확인 요청 알림 |
