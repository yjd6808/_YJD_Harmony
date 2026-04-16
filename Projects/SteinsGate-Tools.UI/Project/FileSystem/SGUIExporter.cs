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
using System.Windows.Media;
using MoreLinq;
using SGToolsCommon.Resource;
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

        private MainViewModel viewModel_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIExporter(MainViewModel _viewModel)
            => viewModel_ = _viewModel;

        //////////////////////////////////////////////////////////////////////////////////
        private string DefineNamePrefix(SGUIElement _element)
        {
            string prefix = string.Empty;

            _element.ParentTrack.Reverse().ForEach(parent =>
            {
                if (parent == viewModel_.GroupMaster)
                    return;

                if (parent.DefineName.Length > 0)
                    prefix += parent.DefineName + "_";
            });

            return prefix;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool Export(string _path)
        {
            string fileName = Path.GetFileName(_path);

            try
            {
                string? dir = Path.GetDirectoryName(_path);
                if (!Directory.Exists(dir))
                    throw new Exception("해당 경로를 저장할 수 없습니다.");

                Dictionary<string, bool> uniqueDefineNameMap = new();
                List<string> duplicatedDefineNameList = new();
                const bool PLACEHOLDER = false;

                using (FileStream fs = new FileStream(_path, FileMode.Create, FileAccess.Write))
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

                        if (!uniqueDefineNameMap.TryAdd(defName, PLACEHOLDER))
                            duplicatedDefineNameList.Add(defName);
                    }

                    WriteDefine(viewModel_.GroupMaster, true);
                    writer.WriteLine("// ========================================");
                    writer.WriteLine();
                    viewModel_.GroupMaster.ForEachRecursive(element => WriteDefine(element, false));
                }
                StringBuilder builder = new StringBuilder(60 * duplicatedDefineNameList.Count);

                for (int i = 0; i < duplicatedDefineNameList.Count; ++i)
                    builder.AppendLine($"[{i}]: {duplicatedDefineNameList[i]}");

                viewModel_.LogBox.AddDispatchedLog($"헤더 추출완료 {fileName} (중복:{duplicatedDefineNameList.Count})\n{builder}", (LogType.Path, (object)_path), IconCommonType.Checked, Brushes.MediumPurple);

                return true;
            }
            catch (Exception e)
            {
                viewModel_.LogBox.AddDispatchedLog(e);
                return false;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public async Task<bool> ExportAsync(string? _path = null)
        {
            if (_path == null)
                _path = Path.Combine(viewModel_.Setting.OutputDefineHeaderFilePath, viewModel_.Setting.OutputDefineHeaderFileName);

            return await Task.Run(() => Export(_path));
        }
    }
}
