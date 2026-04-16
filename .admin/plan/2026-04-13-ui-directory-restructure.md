# UI 프로젝트 디렉토리 구조 변경 계획

## 날짜
2026-04-13

## 목표
`Projects/SteinsGate-Tools.UI/Project` 폴더를  
`RICControllerSharp.View/Customize` 참고 구조와 동일하게 재편성

## 참고 구조 (RICControllerSharp.View)
```
Customize/
├── Behavior/
├── Control/
├── Converter/
├── Property/
├── Provider/
├── Style/
├── Control.xaml
├── Converter.xaml
└── Style.xaml
```

## 현재 → 변경 후 매핑

| 현재 폴더 | 변경 후 폴더 |
|-----------|-------------|
| `CustomControl/` | `Customize/Control/` |
| `CustomStyle/` | `Customize/Style/` |
| `Converter/` | `Customize/Converter/` |
| `View/` | `Customize/View/` |
| (없음) | `Customize/Behavior/` (신규) |
| (없음) | `Customize/Property/` (신규) |
| Themes/Generic.xaml (향후) | `Customize/Style/` |

## 네임스페이스 변경

| 현재 | 변경 후 |
|------|--------|
| `SGToolsUI.CustomControl` | `SGToolsUI.Customize.Control` |
| `SGToolsUI.CustomStyle` | `SGToolsUI.Customize.Style` |
| `SGToolsUI.Converter` | `SGToolsUI.Customize.Converter` |
| `SGToolsUI.View` | `SGToolsUI.Customize.View` |

## 영향받는 파일

### CustomControl → Customize/Control (7개 .cs)
- CanvasShapeItemsControl.cs
- SgaElementListBox.cs
- SgaPackageListBox.cs
- SgaSpriteListBox.cs
- UIElementItemsControl.cs
- UIElementPropertyGrid.cs
- UIElementTreeView.cs

### CustomStyle → Customize/Style (7개 .xaml + 5개 .cs)
- All.xaml, CanvasShapeItemsControl.xaml, MenuItemButton.xaml, ...
- UIElementItemsControlStyle.cs, UIElementPropertyGridStyle.cs, ...

### Converter → Customize/Converter (1개 .xaml + 4개 .cs)
- All.xaml, AbsoluteAnchorPositionThicknessConverter.cs, ...

### View → Customize/View (7개 .xaml + 7개 .cs)
- AlbumView, BackupView, CreateElementView, LogView, MainView, SettingView, SpriteView

## 작업 순서
1. 디렉토리 생성
2. 파일 이동 (git mv)
3. 네임스페이스 업데이트 (.cs 파일)
4. XAML x:Class 및 xmlns 업데이트
5. using 지시문 업데이트
6. 빌드 및 오류 수정
