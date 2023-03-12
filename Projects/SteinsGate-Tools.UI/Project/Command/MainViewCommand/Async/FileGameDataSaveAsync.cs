/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:19:30 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Microsoft.Win32;
using SGToolsCommon.Resource;
using SGToolsUI.FileSystem;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Async
{
    public class FileGameDataSaveAsync : MainCommandAbstractAsync
    {
        public FileGameDataSaveAsync(MainViewModel viewModel)
            : base(viewModel, "UI툴데이터를 저장합니다.")
        {
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            if (parameter is not string param)
                throw new Exception("게임데이터 Save 파라미터가 이상합니다. SaveType 이놈 타입으로 전달해주세요.");

            SaveType saveType = (SaveType)Enum.Parse(typeof(SaveType), param);

            if (saveType == SaveType.Save)
            {
                await ViewModel.Saver.SaveAutoAsync(SaveMode.GameData, false);
            }
            else if (saveType == SaveType.SaveAs)
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Title = "게임데이터 저장";
                saveFileDialog.Filter = "JSON 파일 (*.json)|*.json";
                saveFileDialog.InitialDirectory = Environment.CurrentDirectory;
                saveFileDialog.DefaultExt = Path.GetExtension(Constant.GameDataFileName);
                saveFileDialog.FileName = Path.GetFileNameWithoutExtension(Constant.GameDataFileName);

                if (saveFileDialog.ShowDialog() == true)
                {
                    Exception e = await ViewModel.Saver.SaveGameDataAsync(saveFileDialog.FileName, false);
                    if (e != null) throw e; // MainCommandAbstractAsync에 디폴트 에러 핸들러로 로그박스에서 처리하도록 했으므로 그냥 던져서 위임하면 됨.
                    ViewModel.LogBox.AddLog($"게임데이터 저장완료", (LogType.Path, (object)saveFileDialog.FileName), IconCommonType.Backup, Brushes.RoyalBlue);
                }
            }
            else
            {
                throw new Exception("게임데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }
        }
    }
}
