/*
 * 작성자: 윤정도
 * 생성일: 3/7/2023 4:02:23 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    // 중복까지 고려하긴 시간이 좀 걸릴것같다.
    public class SearchSgaResource : MainCommandAbstract
    {
        private string _beforeSearchText = string.Empty;
        private Dictionary<object, object> _saveDict = new ();  // 연속 검색 저장용

        public SearchSgaResource(MainViewModel viewModel) 
            : base(viewModel, "SgaPackage, SgaImage를 검색합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (parameter is not string searchText)
                return;

            if (_beforeSearchText != searchText)
                _saveDict.Clear();

            _beforeSearchText = searchText;

            SgaManager manager = ViewModel.PackManager;

            ListBox packageListBox = ViewModel.View.PackageListBox;
            List<SgaPackage> packageList = manager.Packages;


            // 엘리먼트 우선검색
            // 시간 널널해지면, 트라이로 구현해볼 것, 성능 훨씬 좋아질듯
            // 근데 지금도 별로 상관없긴하다. 어차피 검색대상이 많아봐야 수천개밖에 안됨. 우선 생각만
            for (int i = 0; i < packageList.Count; ++i)
            {
                SgaPackage package = packageList[i];
                for (int j = 0; j < package.ElementCount; ++j)
                {
                    SgaElementHeader header = package.ElementHeaderList[j];
                    if (!_saveDict.ContainsKey(header) && header.NameWithoutExt.Contains(searchText))
                    {
                        _saveDict.Add(header, header);
                        package.LoadElementIfNotLoaded(header.IndexInPackage, true);
                        SgaImage image = package.GetElement(header.IndexInPackage) as SgaImage;

                        ViewModel.Commander.SelectSgaPackage.Execute(image.Parent);
                        ViewModel.Commander.SelectSgaElement.Execute(header);

                        ViewModel.AlbumView.PackageListBox.ScrollIntoView(image.Parent);
                        packageListBox.ScrollIntoView(image.Parent);

                        ViewModel.View.ElementListBox.ScrollIntoView(header);
                        ViewModel.AlbumView.ElementListBox.ScrollIntoView(header);

                        return;
                    }
                }
            }


            for (int i = 0; i < packageList.Count; ++i)
            {
                SgaPackage package = packageList[i];
                if (!_saveDict.ContainsKey(package) && package.FileNameWithoutExt.Contains(searchText))
                {
                    _saveDict.Add(package, package);
                    ViewModel.Commander.SelectSgaPackage.Execute(package);

                    ViewModel.AlbumView.PackageListBox.SelectedItem = package;
                    ViewModel.AlbumView.PackageListBox.ScrollIntoView(package);

                    packageListBox.SelectedItem = package;
                    packageListBox.ScrollIntoView(package);
                    return;
                }
            }
        }
    }
}
