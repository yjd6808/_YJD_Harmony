/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 10:47:18 AM
 *
 */

using System.Windows;
using System.Windows.Media;

namespace SGToolsUI.Model.Main
{
    public class CanvasAnchor : CanvasEllipse
    {
        private SGUIGroup? target_;

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasAnchor(Rect _rect, int _thickness, Brush _stroke, Brush _fill) : base(_rect, _thickness, _stroke, _fill)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIGroup? Target
        {
            get => target_;
            set
            {
                target_ = value;
                // Target.NotifyProperty(SGUIGroup.VisualPositionAnchorAbsoluteKey);
                OnPropertyChanged();
            }
        }

        public override ShapeElementType ShapeElementType => ShapeElementType.Anchor;
        public override bool IsAnchor => true;
    }
}
