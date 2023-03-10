/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:57:49 AM
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
using SGToolsUI.ViewModel;

namespace SGToolsUI.File
{
    public class SGUIExporter : SGUIFileSystem
    {
        public SGUIExporter(MainViewModel viewModel)
            => _viewModel = viewModel;

        private MainViewModel _viewModel;
    }
}
