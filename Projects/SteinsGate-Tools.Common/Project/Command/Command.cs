/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 4:31:10 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Printing;
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
    public abstract class CommandAbstract : ICommand
    {
        public string Name => GetType().Name;
        public string Description { get; }
        public bool UseParameter { get; set; }


        public CommandAbstract(string description)
        {
            Description = description;
        }

        public bool CanExecute(object? parameter)
        {
            if (UseParameter && parameter == null)
                throw new ArgumentNullException($"{Name} 커맨드의 파라미터는 널일 수 없습니다.");

            return true;
        }
        public abstract void Execute(object? parameter);
        public event EventHandler? CanExecuteChanged;
    }

   
}
