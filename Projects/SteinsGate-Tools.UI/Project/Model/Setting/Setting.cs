/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:35:17 AM
 *
 */

using System;
using System.ComponentModel;
using System.IO;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Primitive;

using IoFile = System.IO.File;

namespace SGToolsUI.Model.Setting
{
    public class Setting : ICloneable
    {
        public const string SgaDirectoryKey = "SgaDirectory";
        public const string OutputXmlLayoutBasePathKey = "OutputXmlLayoutBasePath";
        public const string AutoExpandWhenSelectedKey = "AutoExpandWhenSelected";
        public const string ShowLogViewWhenProgramLaunchedKey = "ShowLogViewWhenProgramLaunched";
        public const string LogViewPositionWhenProgramLaunchedXKey = "LogViewPositionWhenProgramLaunchedX";
        public const string LogViewPositionWhenProgramLaunchedYKey = "LogViewPositionWhenProgramLaunchedY";
        public const string ZoomLevelKey = "ZoomLevel";

        [DisplayName("Sga 패키지 리소스 경로")]
        public string SgaDirectory { get; set; } = "";

        [DisplayName("XML 레이아웃 출력 경로")]
        public string OutputXmlLayoutBasePath { get; set; } = "";

        [DisplayName("아이템 선택시 트리뷰 자동확장 여부")]
        public bool AutoExpandWhenSelected { get; set; } = true;

        [DisplayName("프로그램 실행후 로그뷰 보이기")]
        public bool ShowLogViewWhenProgramLaunched { get; set; } = true;

        [DisplayName("로그뷰 기본 위치")]
        public IntPoint LogViewPositionWhenProgramLaunched { get; set; }

        [DisplayName("초기 배율(%)")]
        public int ZoomLevel { get; set; } = 100;

        //////////////////////////////////////////////////////////////////////////////////
        public object Clone()
        {
            return new Setting()
            {
                SgaDirectory = SgaDirectory,
                OutputXmlLayoutBasePath = OutputXmlLayoutBasePath,
                AutoExpandWhenSelected = AutoExpandWhenSelected,
                ShowLogViewWhenProgramLaunched = ShowLogViewWhenProgramLaunched,
                LogViewPositionWhenProgramLaunched = LogViewPositionWhenProgramLaunched,
                ZoomLevel = ZoomLevel
            };
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool Load()
        {
            if (!IoFile.Exists(Constant.SettingFileName))
                return false;

            string content = IoFile.ReadAllText(Constant.SettingFileName);
            JObject root = JObject.Parse(content);
            JToken? token = null;

            if (root.TryGetValue(SgaDirectoryKey, out token))
                SgaDirectory = (string)token!;

            if (root.TryGetValue(OutputXmlLayoutBasePathKey, out token))
                OutputXmlLayoutBasePath = (string)token!;

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

        //////////////////////////////////////////////////////////////////////////////////
        public void Save()
        {
            JObject root = new JObject();
            root[SgaDirectoryKey] = SgaDirectory;
            root[OutputXmlLayoutBasePathKey] = OutputXmlLayoutBasePath;
            root[AutoExpandWhenSelectedKey] = AutoExpandWhenSelected;
            root[ShowLogViewWhenProgramLaunchedKey] = ShowLogViewWhenProgramLaunched;
            root[LogViewPositionWhenProgramLaunchedXKey] = LogViewPositionWhenProgramLaunched.X;
            root[LogViewPositionWhenProgramLaunchedYKey] = LogViewPositionWhenProgramLaunched.Y;
            root[ZoomLevelKey] = ZoomLevel;
            IoFile.WriteAllText(Constant.SettingFileName, root.ToString());
        }
    }
}
