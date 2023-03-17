/*
 * 작성자: 챗 GPT
 * 생성일: 3/1/2023 2:13:19 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
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
    public class UIElementCanvasSelector : DataTemplateSelector
    {
        public static readonly UIElementCanvasSelector Selector = new ();

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            UIElementItemsControl uiElementItemsControl = container.FindParent<UIElementItemsControl>();

            switch (item)
            {
            case SGUIGroup          __group__       : return (DataTemplate)Application.Current.FindResource("GroupCanvasTemplate");
            case SGUIButton         __button__      : return (DataTemplate)Application.Current.FindResource("ButtonCanvasTemplate");
            case SGUILabel          __label__       : return (DataTemplate)Application.Current.FindResource("LabelCanvasTemplate");
            case SGUISprite         __sprite__      : return (DataTemplate)Application.Current.FindResource("SpriteCanvasTemplate");
            case SGUIEditBox        __editBox__     : return (DataTemplate)Application.Current.FindResource("EditBoxCanvasTemplate");
            case SGUICheckBox       __checkBox__    : return (DataTemplate)Application.Current.FindResource("CheckBoxCanvasTemplate");
            case SGUIToggleButton   __toggleButton__: return (DataTemplate)Application.Current.FindResource("ToggleButtonCanvasTemplate");
            case SGUIScrollBar      __scrollBar__   : return (DataTemplate)Application.Current.FindResource("ScrollBarCanvasTemplate");
            case SGUIProgressBar    __progressBar__ : return (DataTemplate)Application.Current.FindResource("ProgressBarCanvasTemplate");

            }

            throw new Exception("SGUIElementSelector에서 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
