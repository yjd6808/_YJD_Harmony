# SteinsGate-Tools.UI 빌드 경고 0개 달성 계획

## 목표
`SteinsGate-Tools.UI` 프로젝트의 모든 Visual Studio 빌드 경고를 제거하여 경고 0개 달성.

## 수정 규칙 요약
- CS8602 (`possible null dereference`) → `!` 연산자 사용
- CS8600 (`converting null to non-nullable`) → 변수 타입을 nullable로 선언 또는 `!` 추가
- CS8603 (`possible null reference return`) → `return null!;` 사용 (return type 변경 금지 — cascade 발생)
- CS8618 (non-nullable field not initialized) → `null!` 초기화
- CS8625 (`null literal to non-nullable`) → 파라미터 타입을 nullable로 변경
- CS8767 (nullability mismatch override) → 파라미터에 `?` 추가
- CS1998 (async without await) → `async` 제거
- CS0067 (event never used) → `#pragma warning disable/restore CS0067`
- CS0108 (hides inherited member) → `new` 키워드 추가
- CS0162 (unreachable code) → `#pragma warning disable/restore CS0162`

## 기준 경로
`D:\Programming\MyProjects\_DevCpp\UltimateHarmony\Projects\SteinsGate-Tools.UI\Project\`

## 빌드 명령
```
.\Scripts\Build.ps1 -ProjectName SteinsGate-Tools.UI -C Debug -P x64
```
빌드 로그: `D:\Programming\MyProjects\_DevCpp\UltimateHarmony\admin\ui_build.log`

---

## 완료된 파일 (경고 수정 완료)

### FileSystem
- [x] `FileSystem\SGUILoader.cs`
- [x] `FileSystem\SGUISaver.cs`
- [x] `FileSystem\SGUIExporter.cs`

### Model\Main
- [x] `Model\Main\SGUIElement.cs` — Line 1105: `_lhs!.Code.CompareTo(_rhs!.Code)`
- [x] `Model\Main\SGUIGroup.cs`
- [x] `Model\Main\SGUIGroupMaster.cs`
- [x] `Model\Main\SGUIButton.cs`
- [x] `Model\Main\SGUICheckBox.cs` — Lines 223,224,230,231: `Sga!.FileName`, `Img!.Header.Name`
- [x] `Model\Main\SGUIEditBox.cs`
- [x] `Model\Main\SGUILabel.cs`
- [x] `Model\Main\SGUIProgressBar.cs` — Lines 179,189,205: `sprite_.Sprite!.Width/Height/Rect.Size`
- [x] `Model\Main\SGUIScrollBar.cs`
- [x] `Model\Main\SGUISprite.cs` — Lines 62,72,106: `sprite_.Sprite!.Width/Height/Rect.Size`
- [x] `Model\Main\SGUIToggleButton.cs`
- [x] `Model\Main\SGUIStatic.cs`
- [x] `Model\Main\SGUISpriteInfo.cs` — Lines 38,125,133,135,143: `Parent!.Parent`, `null!`, `Sga!`, `Img!`, `Sprite!`
- [x] `Model\Main\CanvasAnchor.cs`
- [x] `Model\Main\CanvasElement.cs`
- [x] `Model\Main\CanvasGrid.cs`
- [x] `Model\Main\CanvasSelection.cs`

### Model\Setting / Backup
- [x] `Model\Setting\Setting.cs`
- [x] `Model\Backup\BackupTreeViewItem.cs`

### Customize\Control
- [x] `Customize\Control\SgaElementListBox.cs`
- [x] `Customize\Control\SgaPackageListBox.cs`
- [x] `Customize\Control\SgaSpriteListBox.cs` — Line 80: `hit.DataContext!`
- [x] `Customize\Control\CanvasShapeItemsControl.cs`
- [x] `Customize\Control\UIElementItemsControl.cs` — Line 753: `SGUIGroup?`, Line 789: `#pragma warning disable CS0067`
- [x] `Customize\Control\UIElementPropertyGrid.cs` — Line 31: `new ContextMenu`, Lines 113,143: `out PropertyInfo?/SGUIElement?`, Line 120: `Img!.Header`, Line 168: `propInfo!.SetValue(element!, ...)`
- [x] `Customize\Control\UIElementTreeView.cs`

### Customize\Style
- [x] `Customize\Style\SgaElementListBoxStyle.cs`
- [x] `Customize\Style\SgaPackageListBoxStyle.cs`
- [x] `Customize\Style\UIElementTreeViewStyle.cs`

### Customize\Converter
- [x] `Customize\Converter\EventModeToStringConverter.cs`
- [x] `Customize\Converter\UIElementTypeBlackImageConverter.cs`
- [x] `Customize\Converter\UIElementTypeColoredImageConverter.cs`

### Command
- [x] `Command\MainCommandAbstractAsync.cs`
- [x] `Command\MainViewCommand\AddUIElement.cs`
- [x] `Command\MainViewCommand\ClipboardOperateUIElement.cs`
- [x] `Command\MainViewCommand\CreateUIElement.cs`
- [x] `Command\MainViewCommand\OpenSettingView.cs`
- [x] `Command\MainViewCommand\SearchSgaResource.cs` — Line 40: `manager.Packages!`
- [x] `Command\MainViewCommand\SelectSgaSprite.cs` — Lines 43,44: `_sprite.Parent!.Parent!`, `_sprite.Parent!.Header`
- [x] `Command\MainViewCommand\Control\CanvasZoomWheel.cs`
- [x] `Command\MainViewCommand\Control\MouseMoveOnWindow.cs`
- [x] `Command\MainViewCommand\Control\SpritePreview.cs`
- [x] `Command\MainViewCommand\Async\FileUIToolDataSaveAsync.cs` — Line 50: `Exception? e`
- [x] `Command\MainViewCommand\Async\FileGameDataSaveAsync.cs` — Line 50: `Exception? e`

### ModelSelector
- [x] `ModelSelector\Backup\BackuprTreeViewItemSelector.cs`
- [x] `ModelSelector\Main\CanvasShapeSelector.cs`
- [x] `ModelSelector\Main\UIElementCanvasSelector.cs`
- [x] `ModelSelector\Main\UIElementTreeViewSelector.cs`

### ModelTemplate
- [x] `ModelTemplate\Main\UIEditBoxCanvasTemplate.cs`
- [x] `ModelTemplate\Main\UISpriteCanvasTemplate.cs`

### ViewModel
- [x] `ViewModel\AlbumViewModel.cs`
- [x] `ViewModel\SettingViewModel.cs`
- [x] `ViewModel\SpriteViewModel.cs` — Line 38: `previewTarget_.Parent!.SpriteList[...]`
- [ ] `ViewModel\MainViewModel.cs` — Line 105: CS0162 (unreachable code) → `#pragma warning disable/restore CS0162` 로 감싸기

### View
- [ ] `View\AlbumView.xaml.cs` — Line 120: `sprite.Parent!.Parent` (CS8602)
- [ ] `View\BackupView.xaml.cs` — 완료 여부 불확실, 재확인 필요
- [ ] `View\CreateElementView.xaml.cs` — Line 83: `DroppedSprite.Parent!` (CS8600)
- [ ] `View\LogView.xaml.cs` — Line 39: `((LogClickEventArgs)_e).LogData!.Data` (CS8602)

---

## 미완료 작업 (다음 세션에서 시작)

### 1. ViewModel\MainViewModel.cs
- Line 105: CS0162 (unreachable code) — `#region DebugElements` 전체를 `#pragma warning disable CS0162` / `restore` 로 감싸기
- 감쌀 범위: line 103 (`#region DebugElements`) ~ line 218 (`#endregion`)

### 2. View\LogView.xaml.cs
- Line 39: `object? data = ((LogClickEventArgs)_e).LogData.Data;`
  → `((LogClickEventArgs)_e).LogData!.Data`

### 3. View\CreateElementView.xaml.cs
- Line 83: `SgaImage img = DroppedSprite.Parent;`
  → `SgaImage img = DroppedSprite.Parent!;`

### 4. View\AlbumView.xaml.cs
- Line 120: `SgaPackage sga = sprite.Parent.Parent;`
  → `SgaPackage sga = sprite.Parent!.Parent;`

### 5. 빌드 수행 및 검증
- `.\Scripts\Build.ps1 -ProjectName SteinsGate-Tools.UI -C Debug -P x64`
- 경고 0개 확인
- 추가로 발생한 경고 있으면 계속 수정

### 6. 작업 기록 저장
- `./admin/histories/2026-04-14-fix-ui-build-warnings.md` 작성


내일 세션 시작 시 다음과 같이 말하면 됩니다:
> ./.admin/plans/2026-04-14-fix-ui-build-warnings.md 파일을 읽고 미완료 작업을 이어서 진행해줘.
현재 남은 작업은:
1. ViewModel\MainViewModel.cs — #region DebugElements 블록을 #pragma warning disable CS0162로 감싸기
2. View\LogView.xaml.cs — .LogData!.Data
3. View\CreateElementView.xaml.cs — DroppedSprite.Parent!
4. View\AlbumView.xaml.cs — sprite.Parent!.Parent
5. 빌드 후 경고 0개 확인
6. 작업 기록(histories) 저장


