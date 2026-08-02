# 2026-08-02 Grid ColumnDefinitions XML + 채널 선택 XML 전환

## 목표
1. **Grid WPF 정식 문법 지원**: `<Grid.ColumnDefinitions><ColumnDefinition Width="*"/>...</Grid.ColumnDefinitions>` 자식 요소 파싱 추가 (RowDefinitions도 동일)
   - 사용자 확정: WPF 정식 문법 / 자식 배치 속성은 기존 `Grid.Row`/`Grid.Column`/`Grid.RowSpan`/`Grid.ColumnSpan` 유지
2. **채널 선택 XML 전환**: `UI_ChannelSelect::BuildCodeFallback` 제거하고 `ui_channel.xml`을 ColumnDefinitions 문법으로 재작성, 채널 목록을 Grid + 부착 속성 배치로 전환

## 현재 상태
- XML 로더(`UIXmlLoader.cpp`):
  - `Columns="*,Auto"` / `Rows="..."` 속성 문법만 지원 (554-578행) → ColumnDefinitions 자식 요소 없음
  - `CreateElement`(304행)가 `ColumnDefinition`/`RowDefinition`/`Grid.ColumnDefinitions` 태그 모름
  - `Grid.Row`/`Grid.Column`/`Grid.RowSpan`/`Grid.ColumnSpan` 부착 속성은 이미 지원 (401-404행)
  - `BuildElement`(616행)가 자식 요소를 전부 `AttachChild`로 부착 → 정의 요소는 부착 대상 제외 처리 필요
- Grid 클래스(`Grid.h`): `AddRowDefinition`/`AddColumnDefinition`/`GetRowCount`/`GetColumnCount` 존재, `ColumnDefinition`/`RowDefinition`/`GridLength` 구조체(`UITypes.h:83-105`) 존재
- 채널 선택(`UI_ChannelSelect.cpp`):
  - `OnInit`이 XML 로드 시 `InitFromXml` / 없으면 `BuildCodeFallback`(229-321행) 폴백
  - `ui_channel.xml`은 `Rows="Auto,Auto,*,Auto"` 속성 문법 사용
  - `BuildChannelButtons`(342행)가 컨테이너 Panel에 버튼 추가만 함 (부착 속성 없음)

## 작업 절차
1. [x] plan 파일 작성 (본 문서)
2. [x] UIXmlLoader.cpp: `ColumnDefinition`/`RowDefinition`/`Grid.ColumnDefinitions`/`Grid.RowDefinitions` 태그 처리
   - `CreateElement`에서 ColumnDefinition/RowDefinition 생성 분기
   - `BuildElement`에서 정의 컬렉션 태그(ColumnDefinitions/RowDefinitions)는 부착 대신 Grid에 등록, ColumnDefinition/RowDefinition은 자식이 아닌 정의로 해석
   - `Width`/`MinWidth`/`MaxWidth`(ColumnDefinition), `Height`/`MinHeight`/`MaxHeight`(RowDefinition) 속성 파싱
3. [x] ui_channel.xml 재작성: `Rows`/`Columns` 속성 → `<Grid.ColumnDefinitions>`/`<Grid.RowDefinitions>` 문법, 채널 목록 Grid에 ColumnDefinitions 정의
4. [x] UI_ChannelSelect.h: `BuildCodeFallback` 선언 제거, `BuildChannelButtons` 컨테이너 타입 Grid로 변경
5. [x] UI_ChannelSelect.cpp: `BuildCodeFallback` 제거, `OnInit`은 XML 전제로 단순화, `BuildChannelButtons`를 Grid + `SetGridColumn`/`SetGridRow` 부착 속성 배치로 전환
6. [x] 빌드 (Build.ps1 sgcl Debug x64) 0 에러
7. [x] 런타임 검증 (start_up_debugging.ps1, `UI_ChannelSelect` OnInit(XML) 로그 / 크래시 없음)
8. [x] plan/progress 업데이트 + /done 문서 작성

## 검증 기준
- [x] XML에서 `<Grid.ColumnDefinitions><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>`로 Grid 정의 로드
- [x] 자식 원소 `Grid.Column="0"` 등 부착 속성 배치 동작 (기존 로직 유지 확인)
- [x] `BuildCodeFallback` 제거 (코드 내 잔여 0건)
- [x] 빌드 0 에러
- [x] 런타임 정상 (채널 선택 XML 로드, 크래시 없음)
