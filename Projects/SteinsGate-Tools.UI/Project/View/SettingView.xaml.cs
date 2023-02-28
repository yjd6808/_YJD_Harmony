/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:43:20 AM
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
using System.Windows.Shapes;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    /// <summary>
    /// Interaction logic for SettingView.xaml
    /// </summary>
    public partial class SettingView : Window
    {
        public SettingViewModel ViewModel { get; }
        public bool Result { get; set; } = false;

        public SettingView()
        {
            ViewModel = new SettingViewModel();
            ViewModel.View = this;
            Resources.Add("ViewModel", ViewModel);
            InitializeComponent();
        }
    }
}
