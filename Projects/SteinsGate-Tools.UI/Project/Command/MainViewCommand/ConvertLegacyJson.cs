using System;
using System.Threading.Tasks;
using System.Windows.Forms;
using SGToolsCommon.Resource;
using SGToolsUI.FileSystem;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ConvertLegacyJson : MainCommandAbstractAsync
    {
        public ConvertLegacyJson(MainViewModel _viewModel)
            : base(_viewModel, "레거시 ui.json을 XML로 변환합니다.")
        {
        }

        public override async Task ExecuteAsync(object? _parameter)
        {
            using var openDialog = new OpenFileDialog();
            openDialog.Title = "레거시 ui.json 파일 선택";
            openDialog.Filter = "JSON 파일 (*.json)|*.json|모든 파일 (*.*)|*.*";
            openDialog.DefaultExt = ".json";

            if (openDialog.ShowDialog() != DialogResult.OK)
                return;

            string jsonPath = openDialog.FileName;

            using var folderDialog = new FolderBrowserDialog();
            folderDialog.Description = "출력 디렉토리 선택";
            folderDialog.ShowNewFolderButton = true;

            if (folderDialog.ShowDialog() != DialogResult.OK)
                return;

            string outputDir = folderDialog.SelectedPath;

            ViewModel.LogBox.AddLog($"레거시 JSON 변환 시작: {jsonPath}", null, IconCommonType.Info, System.Windows.Media.Brushes.Orange);

            try
            {
                await SGUIConverter.ConvertAsync(jsonPath, outputDir);
                ViewModel.LogBox.AddLog($"변환 완료: {outputDir}", null, IconCommonType.Checked, System.Windows.Media.Brushes.Green);
            }
            catch (Exception ex)
            {
                ViewModel.LogBox.AddLog($"변환 실패: {ex.Message}", null, IconCommonType.NotUsable, System.Windows.Media.Brushes.Red);
            }
        }
    }
}
