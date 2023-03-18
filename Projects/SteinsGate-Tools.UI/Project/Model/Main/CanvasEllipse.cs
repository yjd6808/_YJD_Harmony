/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 10:23:06 AM
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
using SGToolsCommon.Extension;

namespace SGToolsUI.Model.Main
{
    public class CanvasEllipse : CanvasShape
    {

        public CanvasEllipse(Rect rect, int thickness, Brush stroke, Brush fill)
        {
            _visualRect = rect;
            _thickness = thickness;
            _strokeBrush = stroke;
            _fillBrush = fill;
        }


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

        public Brush StrokeBrush
        {
            get => _strokeBrush;
            set
            {
                _strokeBrush = value;
                OnPropertyChanged();
            }
        }

        public Brush FillBrush
        {
            get => _fillBrush;
            set
            {
                _strokeBrush = value;
                OnPropertyChanged();
            }
        }

        public int Thickness
        {
            get => _thickness;
            set
            {
                _thickness = value;
                OnPropertyChanged();
            }
        }

        public override ShapeElementType ShapeElementType => ShapeElementType.Ellipse;
        public override bool IsEllipse => true;


        private Rect _visualRect;
        private int _thickness;
        private Brush _strokeBrush;
        private Brush _fillBrush;
    }
}
