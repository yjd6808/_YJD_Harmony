/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:30:58 AM
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
using SGToolsCommon.Extension;
using SGToolsUI.CustomControl;
using SGToolsUI.Model;

namespace SGToolsUI.ModelSelector
{
    public class CanvasShapeSelector : DataTemplateSelector
    {
        public static readonly CanvasShapeSelector Selector = new();

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            CanvasShapeItemsControl cavasShapeItemsControl = container.FindParent<CanvasShapeItemsControl>();

            switch (item)
            {
                case CanvasSelection __selection__: return (DataTemplate)Application.Current.FindResource("CanvasSelectionTemplate");
                case CanvasGrid __grid__: return (DataTemplate)Application.Current.FindResource("CanvasGridTemplate");
                case CanvasRect __rect__: return (DataTemplate)Application.Current.FindResource("CanvasRectTemplate");
                case CanvasAnchor __anchor__: return (DataTemplate)Application.Current.FindResource("CanvasAnchorTemplate");
            }

            throw new Exception("SGUIElementSelector에서 반환가능한 DataTemplate을 찾지 못했습니다.");
        }

    }
}
