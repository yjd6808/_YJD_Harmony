/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 4:23:14 AM
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
    public enum ShapeElementType
    {
        Line,
        Rect,
        Selection,
        Grid,
        Ellipse,
        Anchor
    }


    public abstract class CanvasShape : CanvasElement
    {
        public virtual bool IsSelection => false;
        public virtual bool IsRect => false;
        public virtual bool IsLine => false;
        public virtual bool IsGrid => false;
        public virtual bool IsEllipse => false;
        public virtual bool IsAnchor => false;

        public abstract ShapeElementType ShapeElementType { get; }
        public override CanvasElementType CanvasElementType => CanvasElementType.ShapeElement;
    }
}
