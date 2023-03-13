/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:40:42 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Packaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using MoreLinq.Extensions;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
  

    public partial class AlbumView : Window
    {
        public AlbumViewModel ViewModel { get; }

        public AlbumView(MainViewModel viewModel)
        {
            ViewModel = new AlbumViewModel(this, viewModel);
            InitializeComponent();

            ViewModel.MainViewModel.PackManager.NotifyUpdateList();
        }

        private void PackageListBox_OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SgaPackage previous = e.RemovedItems.Count > 0 ? e.RemovedItems[0] as SgaPackage : null as SgaPackage;
            SgaPackage cur = PackageListBox.SelectedItem as SgaPackage;
            
            if (previous != null) previous.UnloadAll();
            if (cur == null) return;
            ViewModel.SelectedPackage = cur;
        }

        private void ElementListBox_OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SgaElementHeader header = ElementListBox.SelectedItem as SgaElementHeader;
            if (header == null) return;
            SgaPackage package = ViewModel.SelectedPackage;
            if (package == null) return;
            if (!package.HasElement(header.IndexInPackage))
            {
                MessageBoxEx.ShowTopMost($"해당 헤더 인덱스({header.IndexInPackage})에 해당하는 SgaImage가 패키지({package.FileNameWithoutExt})내에 없습니다.");
                return;
            }

            ViewModel.SelectedPackage.LoadElementIfNotLoaded(header.IndexInPackage, false);
            SgaImage cur = ViewModel.SelectedPackage.GetElement(header.IndexInPackage) as SgaImage;
            if (cur == null) return;
            ViewModel.SelectedImage = cur;
            // ViewModel.SelectedImage.NotifyUpdateList();
        }
    }
}
