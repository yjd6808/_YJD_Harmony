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

namespace SGToolsUI.File
{
    public enum SaveType
    {
        Save,
        SaveAs
    }

    public class SGUIFileSystem
    {
        public const string SaveKey = "Save";
        public const string SaveAsKey = "SaveAs";

        public const string JsonModeKey = "mode";
        public const string JsonDateKey = "date";
        public const string JsonGroupKey = "groups";
        public const string JsonGroupMasterKey = "group_master";
        public const string JsonElementKey = "elements";
    }
}
