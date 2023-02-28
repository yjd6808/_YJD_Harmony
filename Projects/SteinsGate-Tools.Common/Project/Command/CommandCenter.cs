/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 4:48:27 AM
 *
 * 에씨비 그두고썰
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

namespace SGToolsCommon.Command
{
    public abstract class CommandCenter
    {
        private Dictionary<string, CommandAbstract> _commandMap = new();

        public void Add(CommandAbstract command)
        {
            if (_commandMap.ContainsKey(command.Name))
            {
                MessageBox.Show($"{command.Name} 커맨드가 이미 존재합니다.");
                return;
            }

            _commandMap.Add(command.Name, command);
        }

        public void Execute(string commandName, object? param = null)
        {
            if (!_commandMap.ContainsKey(commandName))
            {
                MessageBox.Show($"{commandName} 커맨드를 실행할 수 없습니다.");
                return;
            }

            _commandMap[commandName].Execute(param);
        }
    }
}
