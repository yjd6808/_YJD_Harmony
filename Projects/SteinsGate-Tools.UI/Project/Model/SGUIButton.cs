﻿/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:23:45 AM
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
using SGToolsCommon.Sga;

namespace SGToolsUI.Model
{


    public class SGUIButton : SGUIElement
    {
        public SGUISpriteInfo Normal
        {
            get => _normal;
            set
            {
                _normal = value;
                OnPropertyChanged();
            }
        }

        public SGUISpriteInfo Pressed
        {
            get => _pressed;
            set
            {
                _pressed = value;
                OnPropertyChanged();
            }
        }

        public SGUISpriteInfo Over
        {
            get => _over;
            set
            {
                _over = value;
                OnPropertyChanged();
            }
        }

        public SGUISpriteInfo Disabled
        {
            get => _disabled;
            set
            {
                _disabled = value;
                OnPropertyChanged();
            }
        }

     
        private SGUISpriteInfo _normal;
        private SGUISpriteInfo _pressed;
        private SGUISpriteInfo _over;
        private SGUISpriteInfo _disabled;

        public override SGUIElementType UIElementType => SGUIElementType.Button;

        public override void CreateInit()
        {
            VisualName = $"버튼_{Seq++}";
        }

        public override object Clone()
        {
            SGUIButton button = new SGUIButton();
            button.CopyFrom(this);
            button.Normal = Normal;
            button.Pressed = Pressed;
            button.Over = Over;
            button.Disabled = Disabled;
            return button;
        }

        public static int Seq = 0;
    }
}
