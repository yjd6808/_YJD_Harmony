/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:35:17 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using Newtonsoft.Json.Linq;
using SGToolsCommon.Primitive;

namespace SGToolsUI.Model
{
    public class Setting : ICloneable
    {

        [DisplayName("Sga 패키지 리소스 경로")]
        public string SgaDirectory { get; set; } = "";

        [DisplayName("UIDefine.h 출력경로")]
        public string OutputDefinePath { get; set; } = "";

        [DisplayName("ui.json 출력 경로")]
        public string OutputJsonPath { get; set; } = "";

        [DisplayName("아이템 선택시 트리뷰 자동확장 여부")]
        public bool AutoExpandWhenSelected { get; set; } = true;

        [DisplayName("프로그램 실행후 로그뷰 보이기")]
        public bool ShowLogViewWhenProgramLaunched { get; set; } = true;

        [DisplayName("로그뷰 기본 위치")]
        public Point LogViewPositionWhenProgramLaunched { get; set; } 


        public object Clone()
        {
            return new Setting()
            {
                SgaDirectory = SgaDirectory,
                OutputDefinePath = OutputDefinePath,
                OutputJsonPath = OutputJsonPath,
                AutoExpandWhenSelected = AutoExpandWhenSelected,
                ShowLogViewWhenProgramLaunched = ShowLogViewWhenProgramLaunched,
                LogViewPositionWhenProgramLaunched = LogViewPositionWhenProgramLaunched
            };
        }

        public bool Load()
        {
            if (!File.Exists(Constant.SettingFileName))
                return false;

            string content = File.ReadAllText(Constant.SettingFileName);
            JObject root = JObject.Parse(content);
            JToken? token = null;

            if (root.TryGetValue("SgaDirectory", out token))
                SgaDirectory = (string)token;

            if (root.TryGetValue("OutputDefinePath", out token))
                OutputDefinePath = (string)token;

            if (root.TryGetValue("OutputJsonPath", out token))
                OutputJsonPath = (string)token;

            if (root.TryGetValue("AutoExpandWhenSelected", out token))
                AutoExpandWhenSelected = (bool)token;

            if (root.TryGetValue("ShowLogViewWhenProgramLaunched", out token))
                ShowLogViewWhenProgramLaunched = (bool)token;

            Point temp = new ();

            if (root.TryGetValue("LogViewPositionWhenProgramLaunchedX", out token))
                temp.X = (int)token;

            if (root.TryGetValue("LogViewPositionWhenProgramLaunchedY", out token))
                temp.Y = (int)token;

            LogViewPositionWhenProgramLaunched = temp;
            return true;
        }

        public void Save()
        {
            JObject root = new JObject();
            root["SgaDirectory"] = SgaDirectory;
            root["OutputDefinePath"] = OutputDefinePath;
            root["OutputJsonPath"] = OutputJsonPath;
            root["AutoExpandWhenSelected"] = AutoExpandWhenSelected;
            root["ShowLogViewWhenProgramLaunched"] = ShowLogViewWhenProgramLaunched;
            root["LogViewPositionWhenProgramLaunchedX"] = LogViewPositionWhenProgramLaunched.X;
            root["LogViewPositionWhenProgramLaunchedY"] = LogViewPositionWhenProgramLaunched.Y;
            File.WriteAllText(Constant.SettingFileName, root.ToString());
        }
    }
}
