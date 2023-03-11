/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:55:35 AM
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
using Microsoft.Win32;
using SGToolsCommon.Resource;
using SGToolsUI.File;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Async
{
    public class FileGameHeaderExportAsync : MainCommandAbstractAsync
    {
        public FileGameHeaderExportAsync(MainViewModel viewModel)
            : base(viewModel, "UI툴 데이터의 디파인을 추출하여 저장합니다.")
        {
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            if (parameter is not string param)
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
                {
                    await ViewModel.Exporter.ExportAsync(saveFileDialog.FileName);
                }
            }
            else
            {
                throw new Exception("UI툴데이터 저장 파라미터가 이상합니다. (2) 문자열 0 또는 1");
            }
        }
    }
}
