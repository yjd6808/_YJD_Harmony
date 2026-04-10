# C# 코딩 컨벤션 리팩토링 진행 상태

## 작업 개요
SteinsGate-Tools* 3개 프로젝트 전체 C# 파일에 대해 코딩 컨벤션 적용

## 컨벤션 요약
- 멤버 필드: suffix `_` + camelCase (예: `health_`, `commandMap_`)
- 파라미터: prefix `_` + camelCase (예: `_data`, `_result`)
- 지역 변수: camelCase (prefix/suffix 없음)
- 함수 구분자: `//////////////////////////////////////////////////////////////////////////////////` (클래스 내부 = 82자)
- override 메서드: `virtual override` 모두 명시
- abstract 구현: `override`만
- using: System.* → System.Windows.* → NuGet → 프로젝트 순, 불필요한 제거

## 진행 상황

### SteinsGate-Tools.Common - 완료
- Sga/SgaElement.cs ✅
- Sga/SgaPackage.cs ✅
- Sga/SgaImage.cs ✅
- Sga/SgaSprite.cs ✅
- Sga/SgaSound.cs ✅
- Sga/SgaSpriteAbstract.cs ✅
- Sga/SgaLinkSprite.cs ✅
- Sga/SgaManager.cs ✅
- Sga/SgaLoader.cs ✅
- Sga/SgaColorFormat.cs ✅
- Sga/SgaCompressMode.cs ✅
- Sga/SgaElementHeader.cs ✅
- Sga/SgaElementType.cs ✅
- Sga/SgaPackageType.cs ✅
- Sga/SgaSpriteRect.cs ✅
- Sga/SgaSpriteType.cs ✅
- Project/JobQueue.cs ✅
- Project/DisposeAction.cs ✅
- Project/MouseDownHelper.cs ✅
- Project/SharedResourceDictionary.cs ✅
- CustomControl/TitleBar.xaml.cs ✅
- CustomControl/LogListBox.cs ✅
- CustomControl/StretchingTreeView.cs ✅
- CustomControl/StretchingTreeViewItem.cs ✅
- Extension/ (18개 파일 모두) ✅
- Extension/StreamEx.cs ✅
- Model/DataDragState.cs ✅
- Model/KeyState.cs ✅
- Model/IDataDragReceiver.cs ✅
- Model/IKeyboardInputRecevier.cs ✅
- Primitive/IntPoint.cs ✅
- Primitive/IntRect.cs ✅
- Primitive/IntSize.cs ✅
- Primitive/IntVector.cs ✅
- Primitive/Converter/IntPointConverter.cs ✅
- Primitive/Converter/IntSizeConverter.cs ✅
- Converter/ (13개 파일) ✅
- CustomStyle/CustomStyleKey.cs ✅
- CustomStyle/ImageButtonProperty.cs ✅
- CustomStyle/PlaceholderImageListBoxProperty.cs ✅
- CustomStyle/WinformTreeViewLineConverter.cs ✅
- Provider/ResourcePathProvider.cs ✅
- Resource/R.cs ✅
- ThirdParty/Zlib.cs ✅

### SteinsGate-Tools.UI - 진행중
Command 폴더 완료:
- App.xaml.cs ✅
- Constant.cs ✅
- Command/MainCommandAbstract.cs ✅
- Command/MainCommandAbstractAsync.cs ✅
- Command/MainCommandCenter.cs ✅
- Command/SettingCommandAbstract.cs ✅
- Command/SettingCommandCenter.cs ✅
- Command/SettingViewCommand/Close.cs ✅
- Command/SettingViewCommand/SaveAndClose.cs ✅
- Command/MainViewCommand/AddUIElement.cs ✅
- Command/MainViewCommand/Async/FileGameDataSaveAsync.cs ✅
- Command/MainViewCommand/Async/FileGameHeaderExportAsync.cs ✅
- Command/MainViewCommand/Async/FileUIToolDataBackupAsync.cs ✅
- Command/MainViewCommand/Async/FileUIToolDataLoadAsync.cs ✅
- Command/MainViewCommand/Async/FileUIToolDataSaveAsync.cs ✅
- Command/MainViewCommand/CanvasZoomDown.cs ✅
- Command/MainViewCommand/CanvasZoomReset.cs ✅
- Command/MainViewCommand/CanvasZoomUp.cs ✅
- Command/MainViewCommand/ClipboardOperateUIElement.cs ✅
- Command/MainViewCommand/Control/CanvasZoomWheel.cs ✅
- Command/MainViewCommand/Control/MouseMoveOnWindow.cs ✅
- Command/MainViewCommand/Control/SpritePreview.cs ✅
- Command/MainViewCommand/CreateUIElement.cs ✅
- Command/MainViewCommand/DeleteUIElement.cs ✅
- Command/MainViewCommand/OpenAlbumView.cs ✅
- Command/MainViewCommand/OpenBackupView.cs ✅
- Command/MainViewCommand/OpenDirectory.cs ✅
- Command/MainViewCommand/OpenLogView.cs ✅
- Command/MainViewCommand/OpenSettingView.cs ✅
- Command/MainViewCommand/PickUIElement.cs ✅
- Command/MainViewCommand/PositionUIElement.cs ✅
- Command/MainViewCommand/ReloadSgaPackage.cs ✅
- Command/MainViewCommand/SearchSgaResource.cs ✅
- Command/MainViewCommand/SelectPropertyGridElement.cs ✅
- Command/MainViewCommand/SelectSgaElement.cs ✅
- Command/MainViewCommand/SelectSgaPackage.cs ✅
- Command/MainViewCommand/SelectSgaSprite.cs ✅
- Command/MainViewCommand/SelectUIElement.cs ✅
- Command/MainViewCommand/ShotShortcut.cs ✅
- Command/MainViewCommand/ToggleCanvasAnchor.cs ✅
- Command/MainViewCommand/ToggleCanvasGrid.cs ✅
- Command/MainViewCommand/ToggleCanvasViewport.cs ✅
- Command/MainViewCommand/UnpickUIElement.cs ✅
Converter 폴더 완료:
- Converter/AbsoluteAnchorPositionThicknessConverter.cs ✅
- Converter/EventModeToStringConverter.cs ✅
- Converter/UIElementTypeBlackImageConverter.cs ✅
- Converter/UIElementTypeColoredImageConverter.cs ✅

미완료:
- CustomControl/CanvasShapeItemsControl.cs
- CustomControl/SgaElementListBox.cs
- CustomControl/SgaPackageListBox.cs
- CustomControl/SgaSpriteListBox.cs
- CustomControl/UIElementItemsControl.cs
- CustomControl/UIElementPropertyGrid.cs
- CustomControl/UIElementTreeView.cs
- CustomStyle/SgaElementListBoxStyle.cs
- CustomStyle/SgaPackageListBoxStyle.cs
- CustomStyle/UIElementItemsControlStyle.cs
- CustomStyle/UIElementPropertyGridStyle.cs
- CustomStyle/UIElementTreeViewStyle.cs
- Extension/PointEx.cs
- FileSystem/SGUIExporter.cs
- FileSystem/SGUIFileSystem.cs
- FileSystem/SGUILoader.cs
- FileSystem/SGUISaver.cs
- Model/Backup/BackupFile.cs
- Model/Backup/BackupFolder.cs
- Model/Backup/BackupTreeViewItem.cs
- Model/Main/CanvasAnchor.cs
- Model/Main/CanvasElement.cs
- Model/Main/CanvasEllipse.cs
- Model/Main/CanvasGrid.cs
- Model/Main/CanvasLine.cs
- Model/Main/CanvasRect.cs
- Model/Main/CanvasSelection.cs
- Model/Main/CanvasShape.cs
- Model/Main/SGUIButton.cs
- Model/Main/SGUICheckBox.cs
- Model/Main/SGUIEditBox.cs
- Model/Main/SGUIElement.cs
- Model/Main/SGUIElementType.cs
- Model/Main/SGUIGroup.cs
- Model/Main/SGUIGroupMaster.cs
- Model/Main/SGUILabel.cs
- Model/Main/SGUIProgressBar.cs
- Model/Main/SGUIScrollBar.cs
- Model/Main/SGUISprite.cs
- Model/Main/SGUISpriteInfo.cs
- Model/Main/SGUIStatic.cs
- Model/Main/SGUIToggleButton.cs
- Model/Main/ZoomStates.cs
- Model/Setting/Setting.cs
- ModelSelector/Backup/BackuprTreeViewItemSelector.cs
- ModelSelector/Main/CanvasShapeSelector.cs
- ModelSelector/Main/UIElementCanvasSelector.cs
- ModelSelector/Main/UIElementTreeViewSelector.cs
- ModelTemplate/Main/UIEditBoxCanvasTemplate.cs
- ModelTemplate/Main/UISpriteCanvasTemplate.cs
- Resource/R.cs
- View/AlbumView.xaml.cs
- View/BackupView.xaml.cs
- View/CreateElementView.xaml.cs
- View/LogView.xaml.cs
- View/MainView.xaml.cs
- View/SettingView.xaml.cs
- View/SpriteView.xaml.cs
- ViewModel/AlbumViewModel.cs
- ViewModel/BackupViewModel.cs
- ViewModel/MainViewModel.cs
- ViewModel/SettingViewModel.cs
- ViewModel/SpriteViewModel.cs

### SteinsGate-Tools.Research - 미완료
- App.xaml.cs
- AssemblyInfo.cs
- MainWindow.xaml.cs

## 빌드 상태
아직 빌드 미수행 (모든 파일 처리 후 수행 예정)

## 주의사항
- CanvasShapeItemsControl.cs: 복잡한 파일, 멤버 필드 suffix 변경 필요
- SpritePreview.cs: Dispose() 메서드는 virtual override 사용
- SGUIElement.cs: 핵심 모델, 매우 복잡할 수 있음
- SGUIGroup.cs, SGUIGroupMaster.cs: 복잡한 트리 구조
