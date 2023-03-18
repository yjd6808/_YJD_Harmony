/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:21:01 AM
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
using SGToolsUI.Command;
using SGToolsUI.Model.Setting;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class SettingViewModel
    {
        public SettingViewModel()
        {
            Model = new Setting();
            Model.Load();
            Commander = new SettingCommandCenter(this);
            
        }

        public SettingView View { get; set; }       // Xaml에서 주입
        public Setting Model { get; }  
        public SettingCommandCenter Commander { get; }
    }
}
