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
using SGToolsUI.Model.Main;

namespace SGToolsUI.View
{
    /// <summary>
    /// 모달 윈도우임
    /// </summary>
    public partial class CreateElementView : Window
    {
        public CreateElementView(SGUISpriteInfo info)
        {
            InitializeComponent();
        }

        private void CreateElement_OnClick(object sender, RoutedEventArgs e)
        {
        }
    }
}
