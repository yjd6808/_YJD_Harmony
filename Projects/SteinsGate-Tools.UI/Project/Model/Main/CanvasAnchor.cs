/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 10:47:18 AM
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
    public class CanvasAnchor : CanvasEllipse
    {
        public CanvasAnchor(Rect rect, int thickness, Brush stroke, Brush fill) : base(rect, thickness, stroke, fill)
        {
        }

        public SGUIGroup Target
        {
            get => _target;

            set
            {
                _target = value;
                // Target.NotifyProperty(SGUIGroup.VisualPositionAnchorAbsoluteKey);
                OnPropertyChanged();
            }
        }

        public override ShapeElementType ShapeElementType => ShapeElementType.Anchor;
        public override bool IsAnchor => true;

        private SGUIGroup _target;
    }
}
