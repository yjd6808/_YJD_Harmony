/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 5:37:54 AM
 *
 */

using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Media;

namespace SGToolsUI.Model.Main
{
    public class LinePosition
    {
        public int Position { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public LinePosition(int _pos)
        {
            Position = _pos;
        }
    }

    public class CanvasGrid : CanvasShape
    {
        private Brush foreground_;
        private int thickness_;
        private int interval_;
        private List<LinePosition> lines_ = new();

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasGrid(int _interval, int _thickness, Brush _foreground)
        {
            Interval = _interval;
            thickness_ = _thickness;
            foreground_ = _foreground;
        }

        //////////////////////////////////////////////////////////////////////////////////
        [DisplayName("Interval")]
        [Description("그리드를 구성하는 선들의 간격")]
        public int Interval
        {
            get => interval_;
            set
            {
                if (interval_ == value)
                    return;

                interval_ = value;

                int lineCount = (int)Constant.ResolutionWidth / interval_ + 1;
                var lines = new List<LinePosition>(lineCount);

                for (int i = 0; i < lineCount; ++i)
                    lines.Add(new LinePosition(i * interval_));

                lines_ = lines;
                OnPropertyChanged(nameof(Lines));
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        [DisplayName("Thickness")]
        [Description("그리드를 구성하는 선들의 두께")]
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
        [DisplayName("Thickness")]
        [Description("그리드를 구성하는 선들의 색상")]
        public Brush Foreground
        {
            get => foreground_;
            set
            {
                foreground_ = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public List<LinePosition> Lines => lines_;
        public override bool IsGrid => true;
        public override ShapeElementType ShapeElementType => ShapeElementType.Grid;
    }
}
