/*
 * 작성자: 윤정도
 * 생성일: 10/3/2024 4:59:24 PM
 */

using System.Windows;
using System.Windows.Input;

namespace SGToolsCommon.Customize.Behavior
{
    public static class DisableKeyboardInputBehavior
    {
        public static readonly DependencyProperty IsDisabledProperty =
            DependencyProperty.RegisterAttached(
                "IsDisabled",
                typeof(bool),
                typeof(DisableKeyboardInputBehavior),
                new PropertyMetadata(false, OnIsDisabledChanged));

        public static bool GetIsDisabled(UIElement _element)
        {
            return (bool)_element.GetValue(IsDisabledProperty);
        }

        public static void SetIsDisabled(UIElement _element, bool _value)
        {
            _element.SetValue(IsDisabledProperty, _value);
        }

        private static void OnIsDisabledChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            if (_d is UIElement element)
            {
                if ((bool)_e.NewValue)
                {
                    element.PreviewKeyDown += Element_PreviewKeyDown;
                }
                else
                {
                    element.PreviewKeyDown -= Element_PreviewKeyDown;
                }
            }
        }
        private static void Element_PreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            _e.Handled = true; 
        }
    }
}
