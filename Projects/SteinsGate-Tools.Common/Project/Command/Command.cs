/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 4:31:10 AM
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

namespace SGToolsCommon.Command
{
    public abstract class Command : ICommand
    {
        public string Name { get; }
        public string Decription { get; }


        public Command(string name, string decription)
        {
            Name = name;
            Decription = decription;
        }

        public bool CanExecute(object? parameter)
        {
            return true;
        }

        public abstract void Execute(object? parameter);

        public event EventHandler? CanExecuteChanged;
    }

   
}
