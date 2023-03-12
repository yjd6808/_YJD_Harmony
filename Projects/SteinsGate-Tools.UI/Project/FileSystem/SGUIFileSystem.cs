/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 4:23:52 PM
 *
 */

using System;
using System.Collections.Generic;
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

namespace SGToolsUI.FileSystem
{
    public enum ExportType
    {
        Export,     // 정해진 경로에 파일 추출(UIDefine 경로)
        ExportAs    // 다이얼로그로 선택한 경로에 추출
    }

    public enum LoadType
    {
        Load,       // 정해진 경로의 파일 로딩
        LoadAs      // 다이얼로그로 선택한 파일로딩
    }
    public enum SaveType
    {
        Save,       // 정해진 경로의 파일 로딩(예를들어서 실행파일 경로의 data.json, 게임 데이터 저장 경로)
        SaveAs      // 다이얼로그로 선택한 경로에 저장
    }

    public class SGUIFileSystem
    {
        public const string LoadKey = "Load";
        public const string LoadAsKey = "LoadAs";

        public const string SaveKey = "Save";
        public const string SaveAsKey = "SaveAs";

        public const string ExportKey = "Export";
        public const string ExportAsKey = "ExportAs";

        public const string JsonModeKey = "mode";
        public const string JsonDateKey = "date";
        public const string JsonGroupKey = "groups";
        public const string JsonGroupMasterKey = "group_master";
        public const string JsonElementKey = "elements";
    }
}
