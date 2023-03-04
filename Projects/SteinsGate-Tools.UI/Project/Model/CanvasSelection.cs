
/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 9:40:59 AM
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
    public class CanvasSelection : CanvasShape
    {
        public CanvasSelection(ItemsControl selectionControl)
            => Selection = selectionControl;

        public ItemsControl Selection { get; }
        public SGUIElement SelectedElement { get; set; }
        public override ShapeElementType ShapeElementType => ShapeElementType.Selection;

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

        public override bool IsSelection => true;
        private Rect _visualRect;
    }
}
