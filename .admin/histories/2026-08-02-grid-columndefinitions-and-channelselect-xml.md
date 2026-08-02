# 2026-08-02 Grid ColumnDefinitions XML + 채널 선택 XML 전환 (완료)

## 목표
1. **Grid WPF 정식 문법 지원**: `<Grid.ColumnDefinitions><ColumnDefinition Width="*"/>...</Grid.ColumnDefinitions>` 자식 요소 파싱 추가 (RowDefinitions도 동일)
   - 사용자 확정: WPF 정식 문법 / 자식 배치 속성은 기존 `Grid.Row`/`Grid.Column`/`Grid.RowSpan`/`Grid.ColumnSpan` 유지
2. **채널 선택 XML 전환**: `UI_ChannelSelect::BuildCodeFallback` 제거하고 `ui_channel.xml`을 ColumnDefinitions 문법으로 재작성, 채널 목록을 Grid + 부착 속성 배치로 전환

## 변경 내용

### 1. UIXmlLoader.cpp — Grid 정의 컬렉션 지원
- `ApplyGridDefinitionCollection` 헬퍼 추가: `Grid.ColumnDefinitions`/`Grid.RowDefinitions` 태그 인식
  - `ColumnDefinition`의 `Width`/`MinWidth`/`MaxWidth`, `RowDefinition`의 `Height`/`MinHeight`/`MaxHeight` 파싱
  - `GridLength` 파싱(`*`/Auto/수치) 후 `AddColumnDefinition`/`AddRowDefinition` 호출
- `BuildElement` 자식 순회에 정의 컬렉션 분기 추가: Grid 타입일 때 `Grid.ColumnDefinitions`/`Grid.RowDefinitions`는 자식 부착 대신 정의 등록 처리
- 기존 `Columns="*,Auto"` / `Rows="..."` 속성 문법은 그대로 유지 (하위 호환)

### 2. ui_channel.xml 재작성 (배포 폴더)
- 경로: `C:\Users\jdyun\Desktop\UltimateHarmony_Dev\assets\res_data\layout\ui_channel.xml`
- 루트 Grid: `Rows="..."` 속성 → `<Grid.RowDefinitions>`(Auto/* 조합) 문법 전환
- 채널 목록 Grid: `Columns="..."` 속성 → `<Grid.ColumnDefinitions>` 문법 전환

### 3. UI_ChannelSelect — BuildCodeFallback 제거
- `UI_ChannelSelect.h`: `BuildCodeFallback` 선언 제거, `BuildChannelButtons` 컨테이너 타입을 Panel → Grid로 변경
- `UI_ChannelSelect.cpp`: `BuildCodeFallback`(기존 229-321행) 전체 제거, `OnInit`은 XML 전제(InitFromXml)로 단순화
- `BuildChannelButtons`: `SetGridColumn`/`SetGridRow` 부착 속성 배치로 전환

## 검증 결과
- [x] XML `<Grid.ColumnDefinitions><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>` 로드 동작
- [x] `BuildCodeFallback` 소스 코드 잔여 **0건** (plan/progress 문서 언급만 존재)
- [x] 빌드: `Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64` — **오류 0**
- [x] 런타임: `start_up_debugging.ps1 -ProjectName sgcl` —
  - `[UI_ChannelSelect] channel_list Grid col=2 row=5 children=10` (정의 컬렉션 파싱 + 배치 성공)
  - `[UI_ChannelSelect] OnInit(XML) start=... terminate=... refresh=... teen=... adult=... page=...` (XML 로드 경로)
  - `[UIHost] AddWindow ui_channel rc=2 running=1`, 크래시 없음
