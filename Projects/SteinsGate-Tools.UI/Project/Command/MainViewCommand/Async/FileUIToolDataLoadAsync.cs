/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:12:20 AM
 *
 */

using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.Win32;
using SGToolsUI.FileSystem;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Async
{
    public class FileUIToolDataLoadAsync : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public FileUIToolDataLoadAsync(MainViewModel _viewModel)
            : base(_viewModel, "UI 툴파일을 엽니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            if (_parameter is not string param)
                throw new Exception("UI툴데이터 Load 파라미터가 이상합니다. LoadType 이놈 타입으로 전달해주세요.");

            LoadType saveType = (LoadType)Enum.Parse(typeof(LoadType), param);

            if (saveType == LoadType.Load)
            {
                string loadPath = Path.Combine(Environment.CurrentDirectory, Constant.UIToolDataFileName);
                var loader = new SGUILoader(ViewModel);
                ViewModel.RootGroup = await loader.LoadAsync(loadPath);
            }
            else if (saveType == LoadType.LoadAs)
            {
                OpenFileDialog openFileDialog = new OpenFileDialog();
                openFileDialog.Title = "툴데이터 열기";
                openFileDialog.Filter = "XML 파일 (*.xml)|*.xml";
                openFileDialog.InitialDirectory = Environment.CurrentDirectory;
                openFileDialog.DefaultExt = ".xml";
                openFileDialog.FileName = "data";

                if (openFileDialog.ShowDialog() == true)
                {
                    var loader = new SGUILoader(ViewModel);
                    ViewModel.RootGroup = await loader.LoadAsync(openFileDialog.FileName);
                }
            }
        }
    }
}
