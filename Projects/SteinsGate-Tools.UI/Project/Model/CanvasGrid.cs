/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 5:37:54 AM
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
using Tuple = System.Tuple;

namespace SGToolsUI.Model
{
    public class LinePosition
    {
        public LinePosition(int pos)
        {
            Position = pos;
        }
        public int Position { get; set; }
    }

    public class CanvasGrid : CanvasShape
    {
        public CanvasGrid(int interval, int thickness, Brush foreground)
        {
            Interval = interval;
            _thickness = thickness;
            _foreground = foreground;
        }



        [DisplayName("Interval")]
        [Description("그리드를 구성하는 선들의 간격")]
        public int Interval
        {
            get => _interval;
            set
            {
                if (_interval == value)
                    return;

                _interval = value;

                int lineCount = ((int)Constant.ResolutionWidth) / _interval + 1;
                var lines = new List<LinePosition>(lineCount);

                for (int i = 0; i < lineCount; ++i)
                    lines.Add(new LinePosition(i * _interval));

                _lines = lines;
                OnPropertyChanged("Lines");
                OnPropertyChanged();
            }
        }


        [DisplayName("Thickness")]
        [Description("그리드를 구성하는 선들의 두께")]
        public int Thickness
        {
            get => _thickness;
            set
            {
                _thickness = value;
                
                OnPropertyChanged();
            }
        }


        [DisplayName("Thickness")]
        [Description("그리드를 구성하는 선들의 색상")]
        public Brush Foreground
        {
            get => _foreground;
            set
            {
                _foreground = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public List<LinePosition> Lines => _lines;
        public override bool IsGrid => true;

        public override ShapeElementType ShapeElementType => ShapeElementType.Grid;

        private Brush _foreground;
        public int _thickness;
        private int _interval;
        private List<LinePosition> _lines;
    }
}
