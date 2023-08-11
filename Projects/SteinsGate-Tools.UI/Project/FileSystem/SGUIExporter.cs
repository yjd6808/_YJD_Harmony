/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:57:49 AM
 *
 * 비동기로 수행한다.
 * 따라서 비동기수행중 데이터를 변경하거나 삭제하면 문제가 발생할 수 있으므로 자제하도록.
 * 동기로 하는게 안전하지만 렉걸리는게 싫어서 비동기로 다 바꿈
 * 엘리먼트마다 락을 걸면 신경안써도 되지만 코드가 복잡해질 우려가 있다. 또한, 작업 수행 성능에 영향을 줄 수도 있기 때문이다.
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
using System.Windows.Shapes;
using MoreLinq;
using SGToolsCommon.Resource;
using SGToolsUI.Model;
using SGToolsUI.Model.Main;
using SGToolsUI.View;
using SGToolsUI.ViewModel;
using Path = System.IO.Path;

namespace SGToolsUI.FileSystem
{
    public class SGUIExporter : SGUIFileSystem
    {
        public static readonly string[] DepthStrings = new[]
        {
            "",
            "\t",
            "\t\t",
            "\t\t\t",
            "\t\t\t\t",
            "\t\t\t\t\t",
            "\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t\t\t\t"
        };

        public SGUIExporter(MainViewModel viewModel)
            => _viewModel = viewModel;

        private string DefineNamePrefix(SGUIElement element)
        {
            string prefix = string.Empty;

            element.ParentTrack.Reverse().ForEach(parent =>
            {
                if (parent == _viewModel.GroupMaster)
                    return;

                if (parent.DefineName.Length > 0)
                    prefix += parent.DefineName + "_";
            });

            return prefix;
        }

        public bool Export(string path)
        {
            string fileName = Path.GetFileName(path);

            try
            {
                string dir = Path.GetDirectoryName(path);
                if (!Directory.Exists(dir))
                    throw new Exception("해당 경로를 저장할 수 없습니다.");

                Dictionary<string, bool> uniqueDefineNameMap = new ();
                List<string> duplicatedDefineNameList = new();
                const bool _ = false;

                using (FileStream fs = new FileStream(path, FileMode.Create, FileAccess.Write))
                using (StreamWriter writer = new StreamWriter(fs))
                {
                    void WriteDefine(SGUIElement element, bool groupMaster = false)
                    {
                        if (element.Depth == 0)
                            writer.WriteLine("\n\n");

                        StringBuilder builder = new StringBuilder(128);

                        builder.Append(DefineNamePrefix(element));

                        if (!groupMaster)
                        {
                            builder.Append(element.UIElementType.ToString());
                        }

                        if (element.DefineName.Length > 0)
                        {
                            if (!groupMaster) builder.Append('_');
                            builder.Append(element.DefineName);
                        }

                        int depth = element.Depth;
                        depth = depth < 0 ? 0 : depth;
                        string defName = $"{DepthStrings[depth]}#define {builder.ToString().ToUpper()}";

                        writer.Write(defName);
                        writer.Write('\t');
                        writer.WriteLine(element.Code);

                        if (!uniqueDefineNameMap.TryAdd(defName, _))
                            duplicatedDefineNameList.Add(defName);
                    }

                    WriteDefine(_viewModel.GroupMaster, true);
                    writer.WriteLine("// ========================================");
                    writer.WriteLine();
                    _viewModel.GroupMaster.ForEachRecursive(element => WriteDefine(element, false));
                }
                StringBuilder builder = new StringBuilder(60 * duplicatedDefineNameList.Count);

                for (int i = 0; i < duplicatedDefineNameList.Count; ++i)
                    builder.AppendLine($"[{i}]: {duplicatedDefineNameList[i]}");

                _viewModel.LogBox.AddDispatchedLog($"헤더 추출완료 {fileName} (중복:{duplicatedDefineNameList.Count})\n{builder}", (LogType.Path, (object)path), IconCommonType.Checked, Brushes.MediumPurple);

                return true;
            }
            catch (Exception e)
            {
                _viewModel.LogBox.AddDispatchedLog(e);
                return false;
            }
        }

        public async Task<bool> ExportAsync(string path = null)
        {
            if (path == null)
                path = Path.Combine(_viewModel.Setting.OutputDefineHeaderFilePath, _viewModel.Setting.OutputDefineHeaderFileName);

            return await Task.Run(() => Export(path));
        }

        private MainViewModel _viewModel;
    }
}
