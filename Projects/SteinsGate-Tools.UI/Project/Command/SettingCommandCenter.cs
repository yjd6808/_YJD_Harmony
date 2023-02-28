/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:22:59 AM
 *
 */

using SGToolsCommon.Command;
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
using SGToolsUI.Command.SettingViewCommand;

namespace SGToolsUI.Command
{
    public class SettingCommandCenter : CommandCenter
    {
        public SettingViewModel ViewModel { get; }

        // =================================================
        // 커맨드 목록
        // =================================================
        public CommandAbstract SaveAndClose { get; }
        public CommandAbstract Close { get; }
        public SettingCommandCenter(SettingViewModel viewModel)
        {
            ViewModel = viewModel;

            Add(SaveAndClose = new SaveAndClose(ViewModel));
            Add(Close        = new Close(ViewModel));
        }

    }
}
