/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 6:57:44 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
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
    public class CanvasRect : CanvasShape
    {
        public CanvasRect(Rect rect, int thickness, Brush foreground)
        {
            _visualRect = rect;
            _thickness = thickness;
            _foreground = foreground;
        }


        [Browsable(false)]
        public Rect VisualRect
        {
            get => _visualRect;
            set
            {
                _visualRect = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualPosition));
            }
        }


        [DisplayName(nameof(VisualPosition))]
        [Description("캔버스상의 좌상단 위치")]
        public Point VisualPosition
        {
            get => _visualRect.Location;
            set
            {
                _visualRect.Location = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }


        [DisplayName("VisualSize")]
        [Description("사각형의 크기")]
        public Size VisualSize
        {
            get => _visualRect.Size;
            set
            {
                _visualRect.Size = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }


        [DisplayName("Thickness")]
        [Description("사각형을 외곽선의 두께")]
        public int Thickness
        {
            get => _thickness;
            set
            {
                _thickness = value;
                
                OnPropertyChanged();
            }
        }

        [DisplayName("Foreground")]
        [Description("사각형 외곽선의 색상")]
        public Brush Foreground
        {
            get => _foreground;
            set
            {
                _foreground = value;
                OnPropertyChanged();
            }
        }

        public override bool IsRect => true;
        public override ShapeElementType ShapeElementType => ShapeElementType.Rect;

        private Rect _visualRect;
        private int _thickness;
        private Brush _foreground;
    }
}
