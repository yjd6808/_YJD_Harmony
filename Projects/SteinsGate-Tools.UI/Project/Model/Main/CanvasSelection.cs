/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 9:40:59 AM
 *
 */

using System.Windows.Controls;

namespace SGToolsUI.Model.Main
{
    public class CanvasSelection : CanvasShape
    {
        private SGUIElement element_;

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasSelection(ItemsControl _selectionControl)
            => Selection = _selectionControl;

        //////////////////////////////////////////////////////////////////////////////////
        public ItemsControl Selection { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIElement Element
        {
            get => element_;
            set
            {
                element_ = value;
                OnPropertyChanged();
            }
        }

        public override ShapeElementType ShapeElementType => ShapeElementType.Selection;
        public override bool IsSelection => true;
    }
}
