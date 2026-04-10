/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 6:57:44 AM
 *
 */

using System.ComponentModel;
using System.Windows;
using System.Windows.Media;
using SGToolsCommon.Primitive;

namespace SGToolsUI.Model.Main
{
    public class CanvasRect : CanvasShape
    {
        private IntRect visualRect_;
        private int thickness_;
        private Brush foreground_;

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasRect(Rect _rect, int _thickness, Brush _foreground)
        {
            visualRect_ = _rect;
            thickness_ = _thickness;
            foreground_ = _foreground;
        }

        //////////////////////////////////////////////////////////////////////////////////
        [Browsable(false)]
        public IntRect VisualRect
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
        [DisplayName(nameof(VisualPosition))]
        [Description("캔버스상의 좌상단 위치")]
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
        [DisplayName("VisualSize")]
        [Description("사각형의 크기")]
        public IntSize VisualSize
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
        [DisplayName("Thickness")]
        [Description("사각형을 외곽선의 두께")]
        public int Thickness
        {
            get => thickness_;
            set
            {
                thickness_ = value;
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        [DisplayName("Foreground")]
        [Description("사각형 외곽선의 색상")]
        public Brush Foreground
        {
            get => foreground_;
            set
            {
                foreground_ = value;
                OnPropertyChanged();
            }
        }

        public override bool IsRect => true;
        public override ShapeElementType ShapeElementType => ShapeElementType.Rect;
    }
}
