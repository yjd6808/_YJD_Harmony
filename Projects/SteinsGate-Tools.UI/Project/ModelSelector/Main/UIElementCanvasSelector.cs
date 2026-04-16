/*
 * 작성자: 챗 GPT
 * 생성일: 3/1/2023 2:13:19 PM
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
    public class UIElementCanvasSelector : DataTemplateSelector
    {
        public static readonly UIElementCanvasSelector Selector = new();

        //////////////////////////////////////////////////////////////////////////////////
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            UIElementItemsControl? uiElementItemsControl = container.FindParent<UIElementItemsControl>();

            switch (item)
            {
                case SGUIGroup uiGroup: return (DataTemplate)Application.Current.FindResource("GroupCanvasTemplate");
                case SGUIButton uiButton: return (DataTemplate)Application.Current.FindResource("ButtonCanvasTemplate");
                case SGUILabel uiLabel: return (DataTemplate)Application.Current.FindResource("LabelCanvasTemplate");
                case SGUISprite uiSprite: return (DataTemplate)Application.Current.FindResource("SpriteCanvasTemplate");
                case SGUIEditBox uiEditBox: return (DataTemplate)Application.Current.FindResource("EditBoxCanvasTemplate");
                case SGUICheckBox uiCheckBox: return (DataTemplate)Application.Current.FindResource("CheckBoxCanvasTemplate");
                case SGUIToggleButton uiToggleButton: return (DataTemplate)Application.Current.FindResource("ToggleButtonCanvasTemplate");
                case SGUIScrollBar uiScrollBar: return (DataTemplate)Application.Current.FindResource("ScrollBarCanvasTemplate");
                case SGUIProgressBar uiProgressBar: return (DataTemplate)Application.Current.FindResource("ProgressBarCanvasTemplate");
                case SGUIStatic uiStatic: return (DataTemplate)Application.Current.FindResource("StaticCanvasTemplate");
            }

            throw new Exception("SGUIElementSelector에서 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
