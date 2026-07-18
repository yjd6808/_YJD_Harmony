# LogView 시작 시 자동 열림 방지

## 변경 내용
- `MainViewModel.cs` — 생성자에서 `LogView.Show()` 및 위치 설정 코드 제거 (lines 88-92)
  - `if (Setting.ShowLogViewWhenProgramLaunched) LogView.Show();`
  - `if (Setting.ShowLogViewWhenProgramLaunched && Setting.LogViewPositionWhenProgramLaunched != IntPoint.Zero) LogView.MoveTo(...)`

## 결과
- LogView는 메뉴(로그 뷰) 또는 단축키로만 열림
- 시작 시 자동으로 열리지 않음
- `ShowLogViewWhenProgramLaunched` 설정값은 유지 (추후 UI에서 옵션 제거 가능)

## 빌드
- `SteinsGate-Tools.UI Debug x64` — 성공
