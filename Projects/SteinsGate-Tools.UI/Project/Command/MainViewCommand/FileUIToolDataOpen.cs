/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:12:20 AM
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
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using Microsoft.Win32;
using SGToolsUI.File;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class FileUIToolDataOpen : MainCommandAbstract
    {
        public FileUIToolDataOpen(MainViewModel viewModel) 
            : base(viewModel, "UI 툴파일을 엽니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "툴데이터 열기";
            openFileDialog.Filter = "JSON 파일 (*.json)|*.json";
            openFileDialog.InitialDirectory = Environment.CurrentDirectory;
            openFileDialog.DefaultExt = Path.GetExtension(Constant.UIToolDataFileName);
            openFileDialog.FileName = Path.GetFileNameWithoutExtension(Constant.UIToolDataFileName);

            if (openFileDialog.ShowDialog() == true)
            {
                SGUIGroupMaster master;

                if (ViewModel.Loader.Load(out master))
                    ViewModel.GroupMaster = master;
            }
        }
    }
}
