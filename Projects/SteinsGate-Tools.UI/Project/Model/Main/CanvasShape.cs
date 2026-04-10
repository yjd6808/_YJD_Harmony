/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 4:23:14 AM
 *
 */

namespace SGToolsUI.Model.Main
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
