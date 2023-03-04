/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 2:22:36 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using SGToolsCommon;

namespace SGToolsUI.Model
{
    

    public class KeyState
    {
        private class KeyElement : Bindable
        {
            public KeyElement(Key key)
                => Key = key;

            public Key Key { get; }
            public bool Pressed
            {
                get => _Pressed;
                set
                {
                    _Pressed = value;
                    OnPropertyChanged();
                }
            }

            private bool _Pressed;
        }

        public KeyState()
        {
            _keyStates = new KeyElement[200];

            for (int i = 0; i < 200; i++)
                _keyStates[i] = new KeyElement((Key)i);
        }

        public bool IsPressed(Key key) => _keyStates[(int)key].Pressed;
        public void Down(Key key) => _keyStates[(int)key].Pressed = true;
        public void Up(Key key) => _keyStates[(int)key].Pressed = false;

        public bool IsShiftPressed => IsPressed(Key.LeftShift) || IsPressed(Key.RightShift);
        public bool IsAltPressed => IsPressed(Key.LeftAlt) || IsPressed(Key.RightAlt);
        public bool IsCtrlPressed => IsPressed(Key.LeftCtrl) || IsPressed(Key.RightCtrl);

        private KeyElement[] _keyStates;


    }
}
