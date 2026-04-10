/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 4:48:27 AM
 *
 * 에씨비 그두고썰
 */

using System;
using System.Collections.Generic;
using System.Windows;

namespace SGToolsCommon.Command
{
    public abstract class CommandCenter
    {
        private bool finalized_;
        private Dictionary<string, CommandAbstract> commandMap_ = new();

        //////////////////////////////////////////////////////////////////////////////////
        public void Add(CommandAbstract _command)
        {
            if (commandMap_.ContainsKey(_command.Name))
            {
                MessageBox.Show($"{_command.Name} 커맨드가 이미 존재합니다.");
                return;
            }

            commandMap_.Add(_command.Name, _command);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Execute(string _commandName, object? _param = null)
        {
            if (finalized_)
                throw new Exception("이미 파이날라이즈드 된 커맨드 센터입니다.");

            if (!commandMap_.ContainsKey(_commandName))
            {
                MessageBox.Show($"{_commandName} 커맨드를 실행할 수 없습니다.");
                return;
            }

            commandMap_[_commandName].Execute(_param);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Finalize()
        {
            foreach (CommandAbstract command in commandMap_.Values)
                command.Dispose();
            finalized_ = true;
        }
    }
}
