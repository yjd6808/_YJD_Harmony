/*
 * 작성자: 윤정도
 * 생성일: 3/7/2023 4:02:23 PM
 *
 */

using System.Collections.Generic;
using System.Windows.Controls;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    // 중복까지 고려하긴 시간이 좀 걸릴것같다.
    public class SearchSgaResource : MainCommandAbstract
    {
        private string beforeSearchText_ = string.Empty;
        private Dictionary<object, object> saveDict_ = new();  // 연속 검색 저장용

        //////////////////////////////////////////////////////////////////////////////////
        public SearchSgaResource(MainViewModel _viewModel)
            : base(_viewModel, "SgaPackage, SgaImage를 검색합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (_parameter is not string searchText)
                return;

            if (searchText.Length == 0 || beforeSearchText_ != searchText)
                saveDict_.Clear();

            beforeSearchText_ = searchText;

            SgaManager manager = ViewModel.PackManager;

            ListBox packageListBox = ViewModel.View.PackageListBox;
            List<SgaPackage> packageList = manager.Packages;

            // 엘리먼트 우선검색
            for (int i = 0; i < packageList.Count; ++i)
            {
                SgaPackage package = packageList[i];
                for (int j = 0; j < package.ElementCount; ++j)
                {
                    SgaElementHeader header = package.ElementHeaderList[j];
                    if (!saveDict_.ContainsKey(header) && header.NameWithoutExt.Contains(searchText))
                    {
                        saveDict_.Add(header, header);
                        package.LoadElementIfNotLoaded(header.IndexInPackage, true);
                        SgaImage image = package.GetElement(header.IndexInPackage) as SgaImage;

                        ViewModel.Commander.SelectSgaPackage.Execute(image.Parent);
                        ViewModel.Commander.SelectSgaElement.Execute(header);

                        packageListBox.ScrollIntoView(image.Parent);
                        ViewModel.View.ElementListBox.ScrollIntoView(header);

                        if (ViewModel.AlbumView.IsVisible)
                        {
                            ViewModel.AlbumView.PackageListBox.ScrollIntoView(image.Parent);
                            ViewModel.AlbumView.ElementListBox.ScrollIntoView(header);
                        }
                        return;
                    }
                }
            }

            for (int i = 0; i < packageList.Count; ++i)
            {
                SgaPackage package = packageList[i];
                if (!saveDict_.ContainsKey(package) && package.FileNameWithoutExt.Contains(searchText))
                {
                    saveDict_.Add(package, package);
                    ViewModel.Commander.SelectSgaPackage.Execute(package);

                    if (ViewModel.AlbumView.IsVisible)
                    {
                        ViewModel.AlbumView.PackageListBox.SelectedItem = package;
                        ViewModel.AlbumView.PackageListBox.ScrollIntoView(package);
                    }

                    packageListBox.SelectedItem = package;
                    packageListBox.ScrollIntoView(package);
                    return;
                }
            }
        }
    }
}
