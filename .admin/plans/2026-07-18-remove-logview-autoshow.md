# LogView 시작 시 자동 열림 방지

## 배경
LogView가 프로그램 시작 시 항상 열리는데, 설정(`ShowLogViewWhenProgramLaunched`)과 무관하게 열리지 않도록 수정 요청.

## 수정 사항
1. `MainViewModel.cs` — 생성자에서 `LogView.Show()` 호출 제거
   - `if (Setting.ShowLogViewWhenProgramLaunched) LogView.Show();` 삭제
   - `if (Setting.ShowLogViewWhenProgramLaunched && Setting.LogViewPositionWhenProgramLaunched != IntPoint.Zero) LogView.MoveTo(...)` 삭제
2. `Setting.cs` — `ShowLogViewWhenProgramLaunched` 속성 및 관련 설정 유지 (다른 곳에서 사용 중), 삭제 불필요

## 영향
- LogView는 메뉴(로그 뷰) 또는 단축키로만 열림
- `ShowLogViewWhenProgramLaunched` 설정값은 무시됨 (추후 설정 UI에서 해당 옵션 제거 가능)
