/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:57:59 AM
 *
 */

using SGToolsUI.ViewModel;
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
using SGToolsUI.Model;
using System.Diagnostics.Metrics;
using SGToolsCommon.Resource;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Newtonsoft.Json.Linq;

namespace SGToolsUI.File
{
    public class SGUILoader
    {
        public SGUILoader(MainViewModel viewModel)
            => _viewModel = viewModel;

        public Exception Load(string path, SGUIGroupMaster master)
        {
            try
            {
                JObject root = JObject.Parse(System.IO.File.ReadAllText(path));

                SaveMode mode = (SaveMode)((int)root["mode"]);
                if (mode == SaveMode.GameData)
                    throw new Exception("게임 데이터를 로딩할려고 하고 있습니다.");

                JArray elements = root["ui_element"] as JArray;
                JArray groups = root["ui_group"] as JArray;



                return null;
            }
            catch (Exception exception)
            {
                return exception;
            }
        }

        public bool Load(out SGUIGroupMaster master)
        {
            master = SGUIGroupMaster.Create(_viewModel);
            Exception e = Load(Constant.UIToolDataFileName, master);

            if (e == null)
                _viewModel.LogBox?.AddLog("데이터 로딩완료", null, IconCommonType.Down, Brushes.DarkCyan);
            else
                _viewModel.LogBox?.AddLog(e);

            return e == null;
        }

        public MainViewModel _viewModel;
    }
}
