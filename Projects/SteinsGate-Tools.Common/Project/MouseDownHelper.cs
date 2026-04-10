/*
 * https://stackoverflow.com/questions/10667545/why-ismouseover-is-recognized-and-mousedown-isnt-wpf-style-trigger
 * 생성일: 2/27/2023 10:40:13 AM
 *
 */

using System.Windows;
using System.Windows.Input;

namespace SGToolsCommon
{
    public static class MouseDownHelper
    {
        public static readonly DependencyProperty IsEnabledProperty = DependencyProperty.RegisterAttached("IsEnabled",
            typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false, new PropertyChangedCallback(OnNotifyPropertyChanged)));

        internal static readonly DependencyPropertyKey IsMouseDownPropertyKey = DependencyProperty.RegisterAttachedReadOnly("IsMouseDown",
            typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false));
        public static readonly DependencyProperty IsMouseDownProperty = IsMouseDownPropertyKey.DependencyProperty;

        internal static readonly DependencyPropertyKey IsMouseLeftButtonDownPropertyKey = DependencyProperty.RegisterAttachedReadOnly("IsMouseLeftButtonDown",
            typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false));
        public static readonly DependencyProperty IsMouseLeftButtonDownProperty = IsMouseLeftButtonDownPropertyKey.DependencyProperty;

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetIsEnabled(UIElement _element, bool _value)
        {
            _element.SetValue(IsEnabledProperty, _value);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool GetIsEnabled(UIElement _element)
        {
            return (bool)_element.GetValue(IsEnabledProperty);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool GetIsMouseDown(UIElement _element)
        {
            return (bool)_element.GetValue(IsMouseDownProperty);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool GetIsMouseLeftButtonDown(UIElement _element)
        {
            return (bool)_element.GetValue(IsMouseLeftButtonDownProperty);
        }

        //////////////////////////////////////////////////////////////////////////////////
        internal static void SetIsMouseDown(UIElement _element, bool _value)
        {
            _element.SetValue(IsMouseDownPropertyKey, _value);
        }

        //////////////////////////////////////////////////////////////////////////////////
        internal static void SetIsMouseLeftButtonDown(UIElement _element, bool _value)
        {
            _element.SetValue(IsMouseLeftButtonDownPropertyKey, _value);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void OnNotifyPropertyChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            UIElement element = _d as UIElement;
            if (element != null && _e.NewValue != null)
            {
                if ((bool)_e.NewValue)
                    Register(element);
                else
                    UnRegister(element);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void Register(UIElement _element)
        {
            _element.PreviewMouseDown += element_MouseDown;
            _element.PreviewMouseLeftButtonDown += element_MouseLeftButtonDown;
            _element.MouseLeave += element_MouseLeave;
            _element.PreviewMouseUp += element_MouseUp;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void UnRegister(UIElement _element)
        {
            _element.PreviewMouseDown -= element_MouseDown;
            _element.PreviewMouseLeftButtonDown -= element_MouseLeftButtonDown;
            _element.MouseLeave -= element_MouseLeave;
            _element.PreviewMouseUp -= element_MouseUp;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void element_MouseDown(object _sender, MouseButtonEventArgs _e)
        {
            UIElement element = _sender as UIElement;
            if (element != null)
                SetIsMouseDown(element, true);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void element_MouseLeftButtonDown(object _sender, MouseButtonEventArgs _e)
        {
            UIElement element = _sender as UIElement;
            if (element != null)
                SetIsMouseLeftButtonDown(element, true);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void element_MouseLeave(object _sender, MouseEventArgs _e)
        {
            UIElement element = _sender as UIElement;
            if (element != null)
            {
                SetIsMouseDown(element, false);
                SetIsMouseLeftButtonDown(element, false);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void element_MouseUp(object _sender, MouseButtonEventArgs _e)
        {
            UIElement element = _sender as UIElement;
            if (element != null)
            {
                SetIsMouseDown(element, false);
                SetIsMouseLeftButtonDown(element, false);
            }
        }
    }
}
