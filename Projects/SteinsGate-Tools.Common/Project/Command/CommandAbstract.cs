/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 4:31:10 AM
 *
 */

using System;
using System.Windows.Input;

namespace SGToolsCommon.Command
{
    public abstract class CommandAbstract : ICommand, IDisposable
    {
        public string Name => GetType().Name;
        public string Description { get; }
        public bool UseParameter { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public CommandAbstract(string _description)
        {
            Description = _description;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool CanExecute(object? _parameter)
        {
            if (UseParameter && _parameter == null)
                throw new ArgumentNullException($"{Name} 커맨드의 파라미터는 널일 수 없습니다.");

            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public abstract void Execute(object? _parameter);

#pragma warning disable CS0067
        public event EventHandler? CanExecuteChanged;
#pragma warning restore CS0067

        //////////////////////////////////////////////////////////////////////////////////
        public virtual void Dispose() { }
    }
}
