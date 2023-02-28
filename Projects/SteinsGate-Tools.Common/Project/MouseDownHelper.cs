﻿/*
 * https://stackoverflow.com/questions/10667545/why-ismouseover-is-recognized-and-mousedown-isnt-wpf-style-trigger
 * 생성일: 2/27/2023 10:40:13 AM
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

namespace SGToolsCommon
{
    public static class MouseDownHelper
    {
        public static readonly DependencyProperty IsEnabledProperty = DependencyProperty.RegisterAttached("IsEnabled",
        typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false, new PropertyChangedCallback(OnNotifyPropertyChanged)));

        public static void SetIsEnabled(UIElement element, bool value)
        {
            element.SetValue(IsEnabledProperty, value);
        }

        public static bool GetIsEnabled(UIElement element)
        {
            return (bool)element.GetValue(IsEnabledProperty);
        }

        private static void OnNotifyPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var element = d as UIElement;
            if (element != null && e.NewValue != null)
            {
                if ((bool)e.NewValue)
                {
                    Register(element);
                }
                else
                {
                    UnRegister(element);
                }
            }
        }

        private static void Register(UIElement element)
        {
            element.PreviewMouseDown += element_MouseDown;
            element.PreviewMouseLeftButtonDown += element_MouseLeftButtonDown;
            element.MouseLeave += element_MouseLeave;
            element.PreviewMouseUp += element_MouseUp;
        }

        private static void UnRegister(UIElement element)
        {
            element.PreviewMouseDown -= element_MouseDown;
            element.PreviewMouseLeftButtonDown -= element_MouseLeftButtonDown;
            element.MouseLeave -= element_MouseLeave;
            element.PreviewMouseUp -= element_MouseUp;
        }

        private static void element_MouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var element = sender as UIElement;
            if (element != null)
            {
                SetIsMouseDown(element, true);
            }
        }

        private static void element_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var element = sender as UIElement;
            if (element != null)
            {
                SetIsMouseLeftButtonDown(element, true);
            }
        }

        private static void element_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            var element = sender as UIElement;
            if (element != null)
            {
                SetIsMouseDown(element, false);
                SetIsMouseLeftButtonDown(element, false);
            }
        }

        private static void element_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var element = sender as UIElement;
            if (element != null)
            {
                SetIsMouseDown(element, false);
                SetIsMouseLeftButtonDown(element, false);
            }
        }

        internal static readonly DependencyPropertyKey IsMouseDownPropertyKey = DependencyProperty.RegisterAttachedReadOnly("IsMouseDown",
        typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false));
        public static readonly DependencyProperty IsMouseDownProperty = IsMouseDownPropertyKey.DependencyProperty;

        internal static void SetIsMouseDown(UIElement element, bool value)
        {
            element.SetValue(IsMouseDownPropertyKey, value);
        }

        public static bool GetIsMouseDown(UIElement element)
        {
            return (bool)element.GetValue(IsMouseDownProperty);
        }

        internal static readonly DependencyPropertyKey IsMouseLeftButtonDownPropertyKey = DependencyProperty.RegisterAttachedReadOnly("IsMouseLeftButtonDown",
        typeof(bool), typeof(MouseDownHelper), new FrameworkPropertyMetadata(false));
        public static readonly DependencyProperty IsMouseLeftButtonDownProperty = IsMouseLeftButtonDownPropertyKey.DependencyProperty;

        internal static void SetIsMouseLeftButtonDown(UIElement element, bool value)
        {
            element.SetValue(IsMouseLeftButtonDownPropertyKey, value);
        }

        public static bool GetIsMouseLeftButtonDown(UIElement element)
        {
            return (bool)element.GetValue(IsMouseLeftButtonDownProperty);
        }
    }
}
