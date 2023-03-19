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

using IoFile = System.IO.File;

namespace SGToolsUI.Model.Setting
{
    public class Setting : ICloneable
    {
        public const string SgaDirectoryKey = "SgaDirectory";
        public const string OutputDefinePathKey = "OutputDefinePath";
        public const string OutputJsonPathKey = "OutputJsonPath";
        public const string AutoExpandWhenSelectedKey = "AutoExpandWhenSelected";
        public const string ShowLogViewWhenProgramLaunchedKey = "ShowLogViewWhenProgramLaunched";
        public const string LogViewPositionWhenProgramLaunchedXKey = "LogViewPositionWhenProgramLaunchedX";
        public const string LogViewPositionWhenProgramLaunchedYKey = "LogViewPositionWhenProgramLaunchedY";
        public const string ZoomLevelKey = "ZoomLevel";


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
        public IntPoint LogViewPositionWhenProgramLaunched { get; set; }
        [DisplayName("초기 배율(%)")]
        public int ZoomLevel { get; set; } = 100;


        public object Clone()
        {
            return new Setting()
            {
                SgaDirectory = SgaDirectory,
                OutputDefinePath = OutputDefinePath,
                OutputJsonPath = OutputJsonPath,
                AutoExpandWhenSelected = AutoExpandWhenSelected,
                ShowLogViewWhenProgramLaunched = ShowLogViewWhenProgramLaunched,
                LogViewPositionWhenProgramLaunched = LogViewPositionWhenProgramLaunched,
                ZoomLevel = ZoomLevel
            };
        }

        public bool Load()
        {
            if (!IoFile.Exists(Constant.SettingFileName))
                return false;

            string content = IoFile.ReadAllText(Constant.SettingFileName);
            JObject root = JObject.Parse(content);
            JToken? token = null;

            if (root.TryGetValue(SgaDirectoryKey, out token))
                SgaDirectory = (string)token;

            if (root.TryGetValue(OutputDefinePathKey, out token))
                OutputDefinePath = (string)token;

            if (root.TryGetValue(OutputJsonPathKey, out token))
                OutputJsonPath = (string)token;

            if (root.TryGetValue(AutoExpandWhenSelectedKey, out token))
                AutoExpandWhenSelected = (bool)token;

            if (root.TryGetValue(ShowLogViewWhenProgramLaunchedKey, out token))
                ShowLogViewWhenProgramLaunched = (bool)token;

            IntPoint temp = new();

            if (root.TryGetValue(LogViewPositionWhenProgramLaunchedXKey, out token))
                temp.X = (int)token;

            if (root.TryGetValue(LogViewPositionWhenProgramLaunchedYKey, out token))
                temp.Y = (int)token;

            LogViewPositionWhenProgramLaunched = temp;

            if (root.TryGetValue(ZoomLevelKey, out token))
                ZoomLevel = (int)token;
            return true;
        }

        public void Save()
        {
            JObject root = new JObject();
            root[SgaDirectoryKey] = SgaDirectory;
            root[OutputDefinePathKey] = OutputDefinePath;
            root[OutputJsonPathKey] = OutputJsonPath;
            root[AutoExpandWhenSelectedKey] = AutoExpandWhenSelected;
            root[ShowLogViewWhenProgramLaunchedKey] = ShowLogViewWhenProgramLaunched;
            root[LogViewPositionWhenProgramLaunchedXKey] = LogViewPositionWhenProgramLaunched.X;
            root[LogViewPositionWhenProgramLaunchedYKey] = LogViewPositionWhenProgramLaunched.Y;
            root[ZoomLevelKey] = ZoomLevel;
            IoFile.WriteAllText(Constant.SettingFileName, root.ToString());
        }
    }
}
