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
using Microsoft.Win32;
using SGToolsUI.File;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class FileGameDataSave : MainCommandAbstract
    {
        public FileGameDataSave(MainViewModel viewModel)
            : base(viewModel, "UI툴데이터를 저장합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (parameter is not string param)
                throw new Exception("게임데이터 저장 파라미터가 이상합니다. 0 또는 1");

            SaveType saveType = (SaveType)Enum.Parse(typeof(SaveType), param);

            if (saveType == SaveType.Save)
            {
                ViewModel.Saver.Save(SaveMode.GameData, false);
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
                    ViewModel.Saver.SaveManual(saveFileDialog.FileName, SaveMode.GameData, false);
                }
            }
            else
            {
                throw new Exception("게임데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }
        }
    }
}
