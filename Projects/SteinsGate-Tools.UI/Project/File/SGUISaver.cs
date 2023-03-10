/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 11:59:25 AM
 *
 * UI 파일 읽고 쓰는 기능담당
 */

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Media;
using MoreLinq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using SGToolsCommon;
using SGToolsUI.ViewModel;
using System.Xml.Linq;
using MoreLinq.Extensions;
using SGToolsCommon.Resource;
using SGToolsUI.Model;

namespace SGToolsUI.File
{
    public enum SaveMode
    {
        UIToolData,   // UI툴에서 실행하게모든 데이터 저장
        GameData      // 게임 구동에 필요한 데이터만 저장
    }

    public class SGUISaver
    {
        public SGUISaver(MainViewModel viewModel)
            => _viewModel = viewModel;

        public Exception Save(string path, SaveMode mode, bool minify = false)
        {
            JObject root = new JObject();
            JArray elements = new JArray();
            JArray groups = new JArray();

            SGUIGroupMaster groupMaster = _viewModel.GroupMaster;

            try
            {
                MoreEnumerable.ForEach(groupMaster.Elements, element => elements.Add(element.ToJObject(mode)));
                MoreEnumerable.ForEach(groupMaster.Groups.Values, group => groups.Add(group.ToJObject(mode)));

                root["date"] = DateTime.Now.ToString();
                root["mode"] = (int)mode;
                root["ui_element"] = elements;
                root["ui_group"] = groups;

                if (minify)
                {
                    JsonSerializerSettings settings = new JsonSerializerSettings
                    {
                        NullValueHandling = NullValueHandling.Ignore,
                        Formatting = Formatting.None // 공백 무시 옵션
                    };

                    System.IO.File.WriteAllText(path, JsonConvert.SerializeObject(root, settings));
                }
                else
                {
                    System.IO.File.WriteAllText(path, root.ToString());
                }

                return null;
            }
            catch (Exception e)
            {
                return e;
            }
        }

        public void Backup(string tag)
        {
            if (!Directory.Exists(Constant.BackupDirectoryRoot))
                Directory.CreateDirectory(Constant.BackupDirectoryRoot);

            DateTime now = DateTime.Now;
            string fileName = now.ToString(Constant.BackupFileFmt) + $"_{tag}.json";
            string savePath = Path.Join(
                Constant.BackupDirectoryRoot,
                now.ToString(Constant.BackupDirectoryFmt),
                fileName
            );

            Exception e = Save(savePath, SaveMode.UIToolData, true);
            
            if (e == null)
                _viewModel.LogBox.AddLog($"백업완료 {fileName}", savePath, IconCommonType.Backup, Brushes.CornflowerBlue);
            else
                _viewModel.LogBox.AddLog(e);
        }

        public void Save(SaveMode mode, bool minify = false)
        {
            string fileName;
            if (mode == SaveMode.UIToolData)
                fileName = Constant.UIToolDataFileName;
            else if (mode == SaveMode.GameData)
                fileName = Path.Combine(_viewModel.Setting.OutputJsonPath, Constant.GameDataFileName);
            else
                throw new Exception("올바르지 않은 모드입니다.");

            Exception e = Save(fileName, mode, minify);

            if (e == null)
                _viewModel.LogBox.AddLog($"{mode} 데이터 저장완료 {fileName}", null, IconCommonType.Backup, Brushes.Gold);
            else
                _viewModel.LogBox.AddLog(e);
            
        }

        public void SaveManual(string fileName, SaveMode mode, bool minify = false)
        {
            Exception e = Save(fileName, mode, minify);

            if (e == null)
                _viewModel.LogBox.AddLog($"{mode} 데이터 수동 저장완료 {fileName}", null, IconCommonType.Down, Brushes.DarkGray);
            else
                _viewModel.LogBox.AddLog(e);
            
        }

        private MainViewModel _viewModel;


    }
}
