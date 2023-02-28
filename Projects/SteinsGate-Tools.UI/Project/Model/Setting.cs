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

namespace SGToolsUI.Model
{
    public class Setting : ICloneable
    {
        [DisplayName("SgaDirectory")]
        [Description("Sga 패키지들이 위치한 디렉토리 경로 정보입니다.")]
        public string SgaDirectory { get; set; } = "";

        [DisplayName("OutputDefinePath")]
        [Description("UIDefine.h 헤더파일이 저장될 경로입니다.")]
        public string OutputDefinePath { get; set; } = "";

        [DisplayName("OutputJsonPath")]
        [Description("ui.json 파일 출력 경로")]
        public string OutputJsonPath { get; set; } = "";

        public object Clone()
        {
            return new Setting()
            {
                SgaDirectory = SgaDirectory,
                OutputDefinePath = OutputDefinePath,
                OutputJsonPath = OutputJsonPath
            };
        }

        public bool Load()
        {
            if (!File.Exists(Constant.SettingFileName))
                return false;

            string content = File.ReadAllText(Constant.SettingFileName);
            JObject root = JObject.Parse(content);

            SgaDirectory = root["SgaDirectory"].ToString();
            OutputDefinePath = root["SgaDirectory"].ToString();
            OutputJsonPath = root["OutputJsonPath"].ToString();
            return true;
        }

        public void Save()
        {
            JObject root = new JObject();
            root["SgaDirectory"] = SgaDirectory;
            root["OutputDefinePath"] = OutputDefinePath;
            root["OutputJsonPath"] = OutputJsonPath;
            File.WriteAllText(Constant.SettingFileName, root.ToString());
        }
    }
}
