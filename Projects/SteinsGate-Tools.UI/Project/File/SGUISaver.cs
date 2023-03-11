/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 11:59:25 AM
 *
 * UI 파일 읽고 쓰는 기능담당
 *
 * 비동기로 수행한다.
 * 따라서 비동기수행중 데이터를 변경하거나 삭제하면 문제가 발생할 수 있으므로 자제하도록.
 * 동기로 하는게 안전하지만 렉걸리는게 싫어서 비동기로 다 바꿈
 * 엘리먼트마다 락을 걸면 신경안써도 되지만 코드가 복잡해질 우려가 있다. 또한, 작업 수행 성능에 영향을 줄 수도 있기 때문이다.
 *
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
using SGToolsCommon.Resource;
using SGToolsUI.Model;
using SGToolsUI.View;
using IoFile = System.IO.File;
namespace SGToolsUI.File
{
    public enum SaveMode
    {
        UIToolData,
        GameData,
        Full
    }

    public class SGUISaver : SGUIFileSystem
    {
        public SGUISaver(MainViewModel viewModel)
            => _viewModel = viewModel;

        public async Task<Exception> SaveAsync(string uiToolDataPath, string gameDataPath = null, bool minify = false)
        {
            // 저장은 다른쓰레드에서 수행토록 한다.
            Exception? e = await Task.Run(async () =>
            {
                JObject root = new JObject();
                JArray elementsRoot = new JArray();
                JArray groupsRoot = new JArray();
                JObject groupMasterRoot;

                SGUIGroupMaster groupMaster = _viewModel.GroupMaster;

                try
                {
                    string uiToolDataDir = uiToolDataPath != null ? Path.GetDirectoryName(uiToolDataPath) : string.Empty;
                    string gameDataDir = gameDataPath != null ? Path.GetDirectoryName(gameDataPath) : string.Empty;

                    if (uiToolDataDir != string.Empty && !Directory.Exists(uiToolDataDir))
                        throw new Exception($"[{uiToolDataDir}] 경로가 존재하지 않습니다. [1]");

                    if (gameDataDir != string.Empty && !Directory.Exists(gameDataDir))
                        throw new Exception($"[{gameDataDir}] 경로가 존재하지 않습니다. [1]");

                    // 다른 쓰레드로 처리하도록 한다. UJ 쓰레드 부하를 최대한 줄이기 위함.
                    groupMaster.ForEachElement(element => elementsRoot.Add(element.ToJObject()));
                    groupMaster.ForEachGroup(group => groupsRoot.Add(group.ToJObject()));

                    
                    root[JsonDateKey] = DateTime.Now.ToString();
                    root[JsonElementKey] = elementsRoot;
                    root[JsonGroupKey] = groupsRoot;
                    root[JsonGroupMasterKey] = groupMasterRoot = groupMaster.ToJObject();

                    JobEvent result;

                    if (uiToolDataPath != null)
                    {
                        root[JsonModeKey] = SaveMode.UIToolData.ToString();
                        SaveJObject(uiToolDataPath, root, minify);
                    }

                    if (gameDataPath != null)
                    {
                        // 비주얼 네임 전부 제거
                        root[JsonModeKey] = SaveMode.GameData.ToString();
                        elementsRoot.ForEach(token => ((JObject)token).Remove(SGUIElement.JsonVisualNameKey));
                        groupsRoot.ForEach(token => ((JObject)token).Remove(SGUIElement.JsonVisualNameKey));
                        groupMasterRoot.Remove(SGUIElement.JsonVisualNameKey);

                        SaveJObject(gameDataPath, root, minify);
                    }

                    return null;
                }
                catch (Exception e)
                {
                    return e;
                }
            });

            return e;
        }

        private void SaveJObject(string path, JObject root, bool minify)
        {
            if (minify)
            {
                JsonSerializerSettings settings = new JsonSerializerSettings
                {
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.None // 공백 무시 옵션
                };

                IoFile.WriteAllText(path, JsonConvert.SerializeObject(root, settings));
            }
            else
            {
                // 스페이스바가 아닌 탭으로 파일 저장
                // https://stackoverflow.com/questions/25788686/how-do-i-save-a-json-file-with-four-spaces-indentation-using-json-net
                using (FileStream fs = IoFile.Open(path, FileMode.Create))
                {
                    using (StreamWriter sw = new StreamWriter(fs))
                    {
                        using (JsonTextWriter jw = new JsonTextWriter(sw))
                        {
                            jw.Formatting = Formatting.Indented;
                            jw.Indentation = 1;
                            jw.IndentChar = '\t';
                            root.WriteTo(jw);
                        }
                    }
                }
            }
        }

        public void Backup(string tag)
        {
            if (!Directory.Exists(Constant.BackupDirectoryRoot))
                Directory.CreateDirectory(Constant.BackupDirectoryRoot);

            DateTime now = DateTime.Now;
            string fileName = now.ToString(Constant.BackupFileFmt) + $"_{tag}.json";
            string savePath = Path.Join(
                Environment.CurrentDirectory,
                Constant.BackupDirectoryRoot,
                now.ToString(Constant.BackupDirectoryFmt),
                fileName
            );
        }

        public async Task<Exception> SaveAutoAsync(SaveMode mode, bool minify)
        {
            string uiToolDataPath = Path.Combine(Environment.CurrentDirectory, Constant.UIToolDataFileName);
            string gameDataPath = Path.Combine(_viewModel.Setting.OutputJsonPath, Constant.GameDataFileName);

            Exception e;
            string msg;
            List<string> paths = new (2);
            switch (mode)
            {
                case SaveMode.UIToolData:
                    e = await SaveAsync(uiToolDataPath, null, minify);
                    msg = "UI툴 데이터";
                    paths.Add(uiToolDataPath);
                    break;
                case SaveMode.GameData:
                    e = await SaveAsync(null, gameDataPath, minify);
                    msg = "게임 데이터";
                    paths.Add(gameDataPath);
                    break;
                case SaveMode.Full:
                    e = await SaveAsync(uiToolDataPath, gameDataPath, minify);
                    msg = "UI툴, 게임데이터";
                    paths.Add(uiToolDataPath);
                    paths.Add(gameDataPath);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(mode), mode, null);
            }

            if (e == null)
                _viewModel.LogBox.AddLog($"{msg} 저장완료", (LogType.MultiPath, (object)paths), IconCommonType.Backup, Brushes.RoyalBlue);
            else
                _viewModel.LogBox.AddLog(e);

            return e;
        }

        public Task<Exception> SaveUIToolDataAsync(string path, bool minify)
        {
            return SaveAsync(path, null, minify);
        }

        public Task<Exception> SaveGameDataAsync(string path, bool minify)
        {
            return SaveAsync(null, path, minify);
        }



        private MainViewModel _viewModel;


    }
}
