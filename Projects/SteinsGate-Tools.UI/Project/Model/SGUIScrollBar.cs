/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 4:55:06 PM
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
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    [CategoryOrder(Constant.ScrollBarCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIScrollBar : SGUIElement
    {
        public const int IndexTrackBackground = 0;
        public const int IndexUpButton = 1;
        public const int IndexTrackButton = 2;
        public const int IndexDownButton = 3;

        public const int OrderTrackBackground = 1;
        public const int OrderUpButton = 2;
        public const int OrderTrackButton = 3;
        public const int OrderDownButton = 4;

        public override SGUIElementType UIElementType => SGUIElementType.ScrollBar;
        public override bool Manipulatable => true;

        public override void CreateInit()
        {
            throw new NotImplementedException();
        }

        public override object Clone()
        {
            throw new NotImplementedException();
        }
    }
}
