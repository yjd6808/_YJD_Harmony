
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

namespace SGToolsUI.Model.Main
{
    public class CanvasSelection : CanvasShape
    {
        public CanvasSelection(ItemsControl selectionControl)
            => Selection = selectionControl;

        public ItemsControl Selection { get; }
        public SGUIElement Element
        {
            get => _element;
            set
            {
                _element = value;
                OnPropertyChanged();
            }
        }
        public override ShapeElementType ShapeElementType => ShapeElementType.Selection;

        public override bool IsSelection => true;

        private SGUIElement _element;
    }
}
