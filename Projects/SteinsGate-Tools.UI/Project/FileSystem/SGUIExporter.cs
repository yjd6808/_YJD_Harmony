﻿/*
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
using SGToolsCommon.Resource;
using SGToolsUI.Model;
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

                using (FileStream fs = new FileStream(path, FileMode.Create, FileAccess.Write))
                using (StreamWriter writer = new StreamWriter(fs))
                {
                    _viewModel.GroupMaster.ForEachRecursive(element =>
                    {
                        if (element.Depth == 0)
                            writer.WriteLine("\n\n");

                        writer.WriteLine($"{DepthStrings[element.Depth]}#define {element.DefineName.ToUpper()}_{element.UIElementType}");

                        if (!uniqueDefineNameMap.TryAdd(element.DefineName, true))
                            duplicatedDefineNameList.Add(element.DefineName);
                    });
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
                path = Path.Combine(_viewModel.Setting.OutputDefinePath, Constant.HeaderFileName);

            return await Task.Run(() => Export(path));
        }

        private MainViewModel _viewModel;
    }
}