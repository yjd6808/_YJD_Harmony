/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:28:28 AM
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
using SGToolsCommon.Command;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public abstract class SettingCommandAbstract : CommandAbstract
    {
        public SettingViewModel ViewModel { get; }

        public SettingCommandAbstract(SettingViewModel viewModel, string description) : base(description)
        {
            ViewModel = viewModel;
        }
    }
}
