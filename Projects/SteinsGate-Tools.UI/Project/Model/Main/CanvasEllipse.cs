/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 10:23:06 AM
 *
 */

using System.ComponentModel;
using System.Windows;
using System.Windows.Media;
using SGToolsCommon.Primitive;

namespace SGToolsUI.Model.Main
{
    public class CanvasEllipse : CanvasShape
    {
        private Rect visualRect_;
        private int thickness_;
        private Brush strokeBrush_;
        private Brush fillBrush_;

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasEllipse(Rect _rect, int _thickness, Brush _stroke, Brush _fill)
        {
            visualRect_ = _rect;
            thickness_ = _thickness;
            strokeBrush_ = _stroke;
            fillBrush_ = _fill;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Rect VisualRect
        {
            get => visualRect_;
            set
            {
                visualRect_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualPosition));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint VisualPosition
        {
            get => visualRect_.Location;
            set
            {
                visualRect_.Location = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Size VisualSize
        {
            get => visualRect_.Size;
            set
            {
                visualRect_.Size = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Brush StrokeBrush
        {
            get => strokeBrush_;
            set
            {
                strokeBrush_ = value;
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Brush FillBrush
        {
            get => fillBrush_;
            set
            {
                strokeBrush_ = value;
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public int Thickness
        {
            get => thickness_;
            set
            {
                thickness_ = value;
                OnPropertyChanged();
            }
        }

        public override ShapeElementType ShapeElementType => ShapeElementType.Ellipse;
        public override bool IsEllipse => true;
    }
}
