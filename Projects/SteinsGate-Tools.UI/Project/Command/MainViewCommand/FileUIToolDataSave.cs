/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:19:17 AM
 *
 */

using Microsoft.Win32;
using SGToolsUI.File;
using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.IO;
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
using System.Windows.Navigation;

namespace SGToolsUI.Command.MainViewCommand
{
    

    public class FileUIToolDataSave : MainCommandAbstract
    {
        public FileUIToolDataSave(MainViewModel viewModel)
            : base(viewModel, "UI툴데이터를 저장합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (parameter is not string param)
                throw new Exception("UI툴데이터 저장 파라미터가 이상합니다. 0 또는 1");

            SaveType saveType = (SaveType)Enum.Parse(typeof(SaveType), param);

            if (saveType == SaveType.Save)
            {
                ViewModel.Saver.Save(SaveMode.UIToolData, false);
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
                    ViewModel.Saver.SaveManual(saveFileDialog.FileName, SaveMode.UIToolData, false);
                }
            }
            else
            {
                throw new Exception("UI툴데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }


           
        }
    }
}
