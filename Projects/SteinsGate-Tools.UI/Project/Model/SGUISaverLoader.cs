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
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;
using SGToolsCommon;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model
{
    public class SGUISaverLoader
    {
        public SGUISaverLoader(MainViewModel viewModel)
            => viewModel = viewModel;

        private void Save(string path)
        {
            JObject root = new JObject();
            JArray elements = new JArray();
            JArray groups = new JArray();

            


            root["date"] = DateTime.Now.ToString();
            root["ui_element"] = elements;
            root["ui_group"] = groups;

            File.WriteAllText(path, root.ToString());
        }

        public void SaveAsync()
        {
            _viewModel.JobQueue
                .Enqueue(() => Save("data.json"), e => _viewModel.LogBox.AddLog(e))
                .ContinueWith((task) =>
                {
                    if (((JobEvent)task.AsyncState).IsSuccess)
                        _viewModel.LogBox.AddLog("성공적으로 저장하였습니다.");
                });
        }

        private MainViewModel _viewModel;

    }
}
