/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:19:17 AM
 *
 */

using System;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Media;
using Microsoft.Win32;
using SGToolsCommon.Resource;
using SGToolsUI.FileSystem;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Async
{
    public class FileUIToolDataSaveAsync : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public FileUIToolDataSaveAsync(MainViewModel _viewModel)
            : base(_viewModel, "UI툴데이터를 저장합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            if (_parameter is not string param)
                throw new Exception("UI툴데이터 Save 파라미터가 이상합니다. SaveType 이놈 타입으로 전달해주세요.");

            SaveType saveType = (SaveType)Enum.Parse(typeof(SaveType), param);

            if (saveType == SaveType.Save)
            {
                await ViewModel.Saver.SaveAutoAsync(SaveMode.UIToolData, false);
            }
            else if (saveType == SaveType.SaveAs)
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Title = "UI툴데이터 저장";
                saveFileDialog.Filter = "JSON 파일 (*.json)|*.json";
                saveFileDialog.InitialDirectory = Environment.CurrentDirectory;
                saveFileDialog.DefaultExt = Path.GetExtension(Constant.UIToolDataFileName);
                saveFileDialog.FileName = Path.GetFileNameWithoutExtension(Constant.UIToolDataFileName);

                if (saveFileDialog.ShowDialog() == true)
                {
                    Exception? e = await ViewModel.Saver.SaveUIToolDataAsync(saveFileDialog.FileName, false);
                    if (e != null) throw e;
                    ViewModel.LogBox.AddLog($"UI툴 데이터 저장완료", (LogType.Path, (object)saveFileDialog.FileName), IconCommonType.Backup, Brushes.RoyalBlue);
                }
            }
            else
            {
                throw new Exception("UI툴데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }
        }
    }
}
