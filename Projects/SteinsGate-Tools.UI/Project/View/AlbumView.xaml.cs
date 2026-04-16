/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:40:42 PM
 *
 */

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using SGToolsCommon.Customize.Control;
using SGToolsCommon.Customize.View;
using SGToolsCommon.Extension;
using SGToolsCommon.Resource;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

using Path = System.IO.Path;

namespace SGToolsUI.View
{
    public partial class AlbumView : Window
    {
        public AlbumViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public AlbumView(MainViewModel _viewModel)
        {
            ViewModel = new AlbumViewModel(this, _viewModel);
            InitializeComponent();

            ViewModel.MainViewModel.PackManager.NotifyUpdateList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void AlbumView_OnLoaded(object _sender, RoutedEventArgs _e)
        {
            SizeToContent = SizeToContent.Manual;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PackageListBox_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            SgaPackage? previous = _e.RemovedItems.Count > 0 ? _e.RemovedItems[0] as SgaPackage : null;
            SgaPackage? cur = PackageListBox.SelectedItem as SgaPackage;

            if (previous != null) previous.UnloadAll();
            if (cur == null) return;
            ViewModel.SelectedPackage = cur;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ElementListBox_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            SgaElementHeader? header = ElementListBox.SelectedItem as SgaElementHeader;
            if (header == null) return;
            SgaPackage package = ViewModel.SelectedPackage;
            if (package == null) return;
            if (!package.HasElement(header.IndexInPackage))
            {
                MessageBoxEx.ShowTopMost($"해당 헤더 인덱스({header.IndexInPackage})에 해당하는 SgaImage가 패키지({package.FileNameWithoutExt})내에 없습니다.");
                return;
            }

            ViewModel.SelectedPackage.LoadElementIfNotLoaded(header.IndexInPackage, false);
            SgaImage? cur = ViewModel.SelectedPackage.GetElement(header.IndexInPackage) as SgaImage;
            if (cur == null) return;
            ViewModel.SelectedImage = cur;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void AlbumListBox_OnPreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            int selectedCount = AlbumListBox.SelectedItems.Count;
            if (selectedCount == 0)
                return;

            LogListBox logBox = ViewModel.MainViewModel.LogBox;
            ProgressView progressView = ViewModel.MainViewModel.ProgressView;

            if (Keyboard.IsKeyDown(Key.LeftCtrl))
            {
                if (Keyboard.IsKeyDown(Key.S))
                {
                    progressView.Show();
                    progressView.MoveToClosestDisplayCenter();
                    int count = 0;
                    string dirPath = Path.Combine(Constant.ImageSaveDirName, (AlbumListBox.SelectedItems[0] as SgaSpriteAbstract)!.DataDir);
                    DirectoryEx.CreateDirectoryIfNotExist(dirPath);
                    foreach (SgaSpriteAbstract selectedItem in AlbumListBox.SelectedItems)
                    {
                        string filePath = Path.Combine(dirPath, selectedItem.ToString() + ".png");
                        await selectedItem.Bitmap.SaveAsync(filePath);
                        count++;
                        double percent = (double)count / selectedCount * 100.0;
                        progressView.ProgressPercentage = percent;
                        progressView.ProgressText = $"진행률 : {percent:F1}%";
                    }
                    logBox.AddLog($"이미지 {selectedCount}개를 저장하였습니다.", (LogType.Path, (object)dirPath), IconCommonType.Info, Brushes.DarkOrchid);
                }
                else if (Keyboard.IsKeyDown(Key.C))
                {
                    (AlbumListBox.SelectedItems[0] as SgaSpriteAbstract)!.Bitmap.SaveToClipboard();
                    logBox.AddLog($"이미지를 클립보드에 저장하였습니다.");
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void AlbumListBox_OnPreviewMouseDoubleClick(object _sender, MouseButtonEventArgs _e)
        {
            int selectedCount = AlbumListBox.SelectedItems.Count;
            if (selectedCount == 0)
                return;
            SgaSpriteAbstract sprite = AlbumListBox.SelectedItems[0] as SgaSpriteAbstract ?? throw new Exception("선택된 리스트박스 아이템을 SgaSprite로 캐스팅하는데 실패했습니다.");
            SgaPackage sga = sprite.Parent!.Parent;
            SgaElementHeader header = sprite.Parent!.Header;

            ViewModel.MainViewModel.Commander.SelectSgaPackage.Execute(sga);
            ViewModel.MainViewModel.Commander.SelectSgaElement.Execute(header);
            ViewModel.MainViewModel.Commander.SelectSgaSprite.Execute(sprite);

            ViewModel.MainViewModel.View.PackageListBox.ScrollIntoView(sga);
            ViewModel.MainViewModel.View.ElementListBox.ScrollIntoView(header);
            ViewModel.MainViewModel.View.SpriteListBox.ScrollIntoView(sprite);

            ViewModel.MainViewModel.View.Topmost = true;
            ViewModel.MainViewModel.View.Topmost = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void AlbumView_OnClosing(object? _sender, CancelEventArgs _e)
        {
            if (!ViewModel.MainViewModel.Terminated)
                Debug.Assert(false);
        }
    }
}
