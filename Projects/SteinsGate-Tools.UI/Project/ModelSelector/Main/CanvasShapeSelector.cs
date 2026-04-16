/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:30:58 AM
 *
 */

using System;
using System.Windows;
using System.Windows.Controls;
using SGToolsCommon.Extension;
using SGToolsUI.Customize.Control;
using SGToolsUI.Model.Main;

namespace SGToolsUI.ModelSelector.Main
{
    public class CanvasShapeSelector : DataTemplateSelector
    {
        public static readonly CanvasShapeSelector Selector = new();

        //////////////////////////////////////////////////////////////////////////////////
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            CanvasShapeItemsControl? cavasShapeItemsControl = container.FindParent<CanvasShapeItemsControl>();

            switch (item)
            {
                case CanvasSelection canvasSelection: return (DataTemplate)Application.Current.FindResource("CanvasSelectionTemplate");
                case CanvasGrid canvasGrid: return (DataTemplate)Application.Current.FindResource("CanvasGridTemplate");
                case CanvasRect canvasRect: return (DataTemplate)Application.Current.FindResource("CanvasRectTemplate");
                case CanvasAnchor canvasAnchor: return (DataTemplate)Application.Current.FindResource("CanvasAnchorTemplate");
            }

            throw new Exception("SGUIElementSelector에서 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
