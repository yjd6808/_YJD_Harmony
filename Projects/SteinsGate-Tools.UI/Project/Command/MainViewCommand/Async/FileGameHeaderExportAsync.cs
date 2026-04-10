/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:55:35 AM
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
    public class FileGameHeaderExportAsync : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public FileGameHeaderExportAsync(MainViewModel _viewModel)
            : base(_viewModel, "UI툴 데이터의 디파인을 추출하여 저장합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            if (_parameter is not string param)
                throw new Exception("게임 헤더 Export 파라미터가 이상합니다. ExportType 이놈 타입으로 전달해주세요.");

            ExportType saveType = (ExportType)Enum.Parse(typeof(ExportType), param);

            if (saveType == ExportType.Export)
            {
                await ViewModel.Exporter.ExportAsync();
            }
            else if (saveType == ExportType.ExportAs)
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Title = "게임헤더 저장";
                saveFileDialog.Filter = "헤더 파일 (*.h)|*.h";
                saveFileDialog.InitialDirectory = Environment.CurrentDirectory;
                saveFileDialog.DefaultExt = Path.GetExtension(Constant.UIToolDataFileName);
                saveFileDialog.FileName = Path.GetFileNameWithoutExtension(Constant.UIToolDataFileName);

                if (saveFileDialog.ShowDialog() == true)
                    await ViewModel.Exporter.ExportAsync(saveFileDialog.FileName);
            }
            else
            {
                throw new Exception("UI툴데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }
        }
    }
}
