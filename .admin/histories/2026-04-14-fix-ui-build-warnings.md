# SteinsGate-Tools.UI 빌드 경고 0개 달성 작업 기록

## 목표
`SteinsGate-Tools.UI` 프로젝트의 모든 C# 빌드 경고를 제거하여 warning CS 0개 달성.

## 완료일
2026-04-16

## 결과
- 빌드 성공
- **C# 컴파일러 경고(warning CS): 0개**
- 잔여 경고: MSBuild/SDK 레벨 3개 (NETSDK1138, MSB3539×2) — 코드 품질 무관

---

## 수정 규칙 요약
- CS8602 (`possible null dereference`) → `!` 연산자 사용
- CS8600 (`converting null to non-nullable`) → 변수 타입을 nullable로 선언 또는 `!` 추가
- CS8603 (`possible null reference return`) → `return null!;` 사용
- CS8604 (`possible null reference argument`) → 인수에 `!` 추가
- CS8618 (non-nullable field not initialized) → `null!` 초기화
- CS8625 (`null literal to non-nullable`) → 파라미터 타입을 nullable로 변경
- CS8767 (nullability mismatch override) → 파라미터에 `?` 추가
- CS1998 (async without await) → `async` 제거
- CS0067 (event never used) → `#pragma warning disable/restore CS0067`
- CS0108 (hides inherited member) → `new` 키워드 추가
- CS0162 (unreachable code) → `#pragma warning disable/restore CS0162`

---

## 수정된 파일 목록

### FileSystem
- `FileSystem\SGUILoader.cs` — Lines 63,64,75,118,119,176,177,189: `!` 추가 (CS8604, CS8600, CS8602)
- `FileSystem\SGUISaver.cs`
- `FileSystem\SGUIExporter.cs`

### Model\Main
- `Model\Main\SGUIElement.cs`
- `Model\Main\SGUIGroup.cs`
- `Model\Main\SGUIGroupMaster.cs`
- `Model\Main\SGUIButton.cs`
- `Model\Main\SGUICheckBox.cs`
- `Model\Main\SGUIEditBox.cs`
- `Model\Main\SGUILabel.cs`
- `Model\Main\SGUIProgressBar.cs`
- `Model\Main\SGUIScrollBar.cs`
- `Model\Main\SGUISprite.cs`
- `Model\Main\SGUIToggleButton.cs`
- `Model\Main\SGUIStatic.cs`
- `Model\Main\SGUISpriteInfo.cs`
- `Model\Main\CanvasAnchor.cs`
- `Model\Main\CanvasElement.cs`
- `Model\Main\CanvasGrid.cs`
- `Model\Main\CanvasSelection.cs`

### Model\Setting / Backup
- `Model\Setting\Setting.cs`
- `Model\Backup\BackupTreeViewItem.cs`

### Customize\Control
- `Customize\Control\SgaElementListBox.cs`
- `Customize\Control\SgaPackageListBox.cs`
- `Customize\Control\SgaSpriteListBox.cs` — Line 80: `ViewModel!.DragState`, `ViewModel!.View` (CS8602)
- `Customize\Control\CanvasShapeItemsControl.cs`
- `Customize\Control\UIElementItemsControl.cs`
- `Customize\Control\UIElementPropertyGrid.cs`
- `Customize\Control\UIElementTreeView.cs`

### Customize\Style
- `Customize\Style\SgaElementListBoxStyle.cs`
- `Customize\Style\SgaPackageListBoxStyle.cs`
- `Customize\Style\UIElementTreeViewStyle.cs`

### Customize\Converter
- `Customize\Converter\EventModeToStringConverter.cs`
- `Customize\Converter\UIElementTypeBlackImageConverter.cs`
- `Customize\Converter\UIElementTypeColoredImageConverter.cs`

### Command
- `Command\MainCommandAbstractAsync.cs`
- `Command\MainViewCommand\AddUIElement.cs`
- `Command\MainViewCommand\ClipboardOperateUIElement.cs`
- `Command\MainViewCommand\CreateUIElement.cs`
- `Command\MainViewCommand\OpenSettingView.cs`
- `Command\MainViewCommand\SearchSgaResource.cs`
- `Command\MainViewCommand\SelectSgaSprite.cs`
- `Command\MainViewCommand\Control\CanvasZoomWheel.cs`
- `Command\MainViewCommand\Control\MouseMoveOnWindow.cs`
- `Command\MainViewCommand\Control\SpritePreview.cs`
- `Command\MainViewCommand\Async\FileUIToolDataSaveAsync.cs`
- `Command\MainViewCommand\Async\FileGameDataSaveAsync.cs`

### ModelSelector
- `ModelSelector\Backup\BackuprTreeViewItemSelector.cs`
- `ModelSelector\Main\CanvasShapeSelector.cs`
- `ModelSelector\Main\UIElementCanvasSelector.cs`
- `ModelSelector\Main\UIElementTreeViewSelector.cs`

### ModelTemplate
- `ModelTemplate\Main\UIEditBoxCanvasTemplate.cs`
- `ModelTemplate\Main\UISpriteCanvasTemplate.cs`

### ViewModel
- `ViewModel\AlbumViewModel.cs`
- `ViewModel\SettingViewModel.cs`
- `ViewModel\SpriteViewModel.cs`
- `ViewModel\MainViewModel.cs` — Lines 103~218: `#pragma warning disable/restore CS0162` (#region DebugElements 블록)

### View
- `View\AlbumView.xaml.cs` — Lines 120,121: `sprite.Parent!.Parent`, `sprite.Parent!.Header` (CS8602)
- `View\BackupView.xaml.cs`
- `View\CreateElementView.xaml.cs` — Line 83: `DroppedSprite.Parent!` (CS8600)
- `View\LogView.xaml.cs` — Line 39: `((LogClickEventArgs)_e).LogData!.Data` (CS8602)
