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
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Media;
using MoreLinq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Resource;
using SGToolsUI.Model.Main;
using SGToolsUI.View;
using SGToolsUI.ViewModel;
using IoFile = System.IO.File;

namespace SGToolsUI.FileSystem
{
    public enum SaveMode
    {
        UIToolData,
        GameData,
        Full
    }

    public class SGUISaver : SGUIFileSystem
    {
        private MainViewModel viewModel_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUISaver(MainViewModel _viewModel)
            => viewModel_ = _viewModel;

        //////////////////////////////////////////////////////////////////////////////////
        public async Task<Exception> SaveAsync(string _uiToolDataPath, string _gameDataPath = null, bool _minify = false)
        {
            // 저장은 다른쓰레드에서 수행토록 한다.
            Exception? e = await Task.Run(async () =>
            {
                JObject root = new JObject();
                JArray elementsRoot = new JArray();
                JArray groupsRoot = new JArray();
                JObject groupMasterRoot;

                SGUIGroupMaster groupMaster = viewModel_.GroupMaster;

                try
                {
                    string uiToolDataDir = _uiToolDataPath != null ? Path.GetDirectoryName(_uiToolDataPath) : string.Empty;
                    string gameDataDir = _gameDataPath != null ? Path.GetDirectoryName(_gameDataPath) : string.Empty;

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

                    if (_uiToolDataPath != null)
                    {
                        root[JsonModeKey] = SaveMode.UIToolData.ToString();
                        SaveJObject(_uiToolDataPath, root, _minify);
                    }

                    if (_gameDataPath != null)
                    {
                        // 비주얼 네임 전부 제거
                        root[JsonModeKey] = SaveMode.GameData.ToString();
                        elementsRoot.ForEach(token =>
                        {
                            ((JObject)token).Remove(SGUIElement.JsonVisualNameKey);
                            ((JObject)token).Remove(SGUIElement.JsonDefineNameKey);
                        });
                        groupsRoot.ForEach(token =>
                        {
                            ((JObject)token).Remove(SGUIElement.JsonVisualNameKey);
                            ((JObject)token).Remove(SGUIElement.JsonDefineNameKey);
                        });

                        groupMasterRoot.Remove(SGUIElement.JsonVisualNameKey);
                        groupMasterRoot.Remove(SGUIElement.JsonDefineNameKey);

                        SaveJObject(_gameDataPath, root, _minify);
                    }

                    return null;
                }
                catch (Exception ex)
                {
                    return ex;
                }
            });

            return e;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SaveJObject(string _path, JObject _root, bool _minify)
        {
            if (_minify)
            {
                JsonSerializerSettings settings = new JsonSerializerSettings
                {
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.None // 공백 무시 옵션
                };

                IoFile.WriteAllText(_path, JsonConvert.SerializeObject(_root, settings));
            }
            else
            {
                // 스페이스바가 아닌 탭으로 파일 저장
                // https://stackoverflow.com/questions/25788686/how-do-i-save-a-json-file-with-four-spaces-indentation-using-json-net
                using (FileStream fs = IoFile.Open(_path, FileMode.Create))
                {
                    using (StreamWriter sw = new StreamWriter(fs))
                    {
                        using (JsonTextWriter jw = new JsonTextWriter(sw))
                        {
                            jw.Formatting = Formatting.Indented;
                            jw.Indentation = 1;
                            jw.IndentChar = '\t';
                            _root.WriteTo(jw);
                        }
                    }
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public async Task BackupAsync(string _tag)
        {
            if (!Directory.Exists(Constant.BackupDirectoryRoot))
                Directory.CreateDirectory(Constant.BackupDirectoryRoot);

            DateTime now = DateTime.Now;
            string fileName = now.ToString(Constant.BackupFileFmt) + $"_{_tag}.json";
            string saveDir = Path.Join(
                Environment.CurrentDirectory,
                Constant.BackupDirectoryRoot,
                now.ToString(Constant.BackupDirectoryFmt)
            );

            DirectoryEx.CreateDirectoryIfNotExist(saveDir);
            string savePath = Path.Join(saveDir, fileName);

            Exception e = await SaveAsync(savePath, null, false);
            if (e == null)
                viewModel_.LogBox.AddDispatchedLog($"{_tag} 백업완료", (LogType.Path, (object)savePath), IconCommonType.Backup, Brushes.RoyalBlue);
            else
                viewModel_.LogBox.AddDispatchedLog(e);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public async Task<Exception> SaveAutoAsync(SaveMode _mode, bool _minify)
        {
            string uiToolDataPath = Path.Combine(Environment.CurrentDirectory, Constant.UIToolDataFileName);
            string gameDataPath = Path.Combine(viewModel_.Setting.OutputJsonPath, Constant.GameDataFileName);

            Exception e;
            string msg;
            List<string> paths = new(2);
            switch (_mode)
            {
                case SaveMode.UIToolData:
                    e = await SaveAsync(uiToolDataPath, null, _minify);
                    msg = "UI툴 데이터";
                    paths.Add(uiToolDataPath);
                    break;
                case SaveMode.GameData:
                    e = await SaveAsync(null, gameDataPath, _minify);
                    msg = "게임 데이터";
                    paths.Add(gameDataPath);
                    break;
                case SaveMode.Full:
                    e = await SaveAsync(uiToolDataPath, gameDataPath, _minify);
                    msg = "UI툴, 게임데이터";
                    paths.Add(uiToolDataPath);
                    paths.Add(gameDataPath);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(_mode), _mode, null);
            }

            if (e == null)
                viewModel_.LogBox.AddLog($"{msg} 저장완료", (LogType.MultiPath, (object)paths), IconCommonType.Backup, Brushes.RoyalBlue);
            else
                viewModel_.LogBox.AddLog(e);

            return e;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Task<Exception> SaveUIToolDataAsync(string _path, bool _minify)
        {
            return SaveAsync(_path, null, _minify);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Task<Exception> SaveGameDataAsync(string _path, bool _minify)
        {
            return SaveAsync(null, _path, _minify);
        }
    }
}
