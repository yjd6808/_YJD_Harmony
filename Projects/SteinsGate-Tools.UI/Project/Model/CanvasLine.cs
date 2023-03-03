/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 4:54:12 AM
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

namespace SGToolsUI.Model
{
    public class CanvasLine : CanvasShape
    {
        public Point Start
        {
            get => _start;
            set
            {
                _start = value;
                OnPropertyChanged();
            }
        }

        public Point End
        {
            get => _end;
            set
            {
                _end = value;
                OnPropertyChanged();
            }
        }


        public override bool IsLine => true;
        public override ShapeElementType ShapeElementType => ShapeElementType.Line;

        private Point _start;
        private Point _end;
    }
}
