/*
 * 작성자: 윤정도
 * 생성일: 10/6/2024 5:12:24 PM
 *
 * 체크박스에 연결된 프레임워크를 클릭하더라도 체크박스를 클릭한 것처럼 동작하다록 하는 비헤비어
 */

using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Behavior
{
    // @참고: https://stackoverflow.com/questions/1448899/attached-property-of-type-list
    // xaml에서는 템플릿 표현이 안되서 이렇게 처리해야함 ㅠ
    public class CheckBoxConnectedElementCollection : ObservableCollection<CheckBoxConnectedElement>{}
    public class CheckBoxConnectedElement : DependencyObject
    {
        public FrameworkElement? TargetElement
        {
            get => GetValue(TargetElementProperty) as FrameworkElement;
            set => SetValue(TargetElementProperty, value);
        }
        public static readonly DependencyProperty TargetElementProperty =
            DependencyProperty.Register(nameof(TargetElement), typeof(FrameworkElement), typeof(CheckBoxConnectedElement));
    }

    public class CheckBoxConnectedElementBehavior
    {
        public static readonly DependencyProperty ElementsProperty =
            DependencyProperty.RegisterAttached(
                "Elements", 
                typeof(CheckBoxConnectedElementCollection), 
                typeof(CheckBoxConnectedElementBehavior), 
                new PropertyMetadata(null, OnElementsChanged));

        public static CheckBoxConnectedElementCollection GetElements(DependencyObject obj)
        {
            return (CheckBoxConnectedElementCollection)obj.GetValue(ElementsProperty);
        }

        public static void SetElements(DependencyObject obj, CheckBoxConnectedElementCollection value)
        {
            obj.SetValue(ElementsProperty, value);
        }

        
        private static void OnElementsChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is not CheckBox checkBox)
                return;
            var newElements = e.NewValue as CheckBoxConnectedElementCollection;
            if (newElements == null)
                return;

            // 중간에 추가되거나 삭제되는 경우는 없을 테지만.. 그냥 추가해둠
            newElements.CollectionChanged += (_sender, _args) =>
            {
                if (_args.Action == NotifyCollectionChangedAction.Add)
                {
                    if (_args.NewItems == null) return;
                    foreach (CheckBoxConnectedElement element in _args.NewItems)
                    {
                        if (element.TargetElement != null)
                        {
                            Debug.Assert(element.TargetElement.Tag == null); // 연결된 엘리먼트 테그에는 체크박스를 임시로 저장함
                            element.TargetElement.Tag = null;
                            element.TargetElement.MouseLeftButtonDown += TargetElementOnMouseLeftButtonDown;
                            element.TargetElement.MouseEnter += TargetElementOnMouseEnter;
                            element.TargetElement.MouseLeave += TargetElementOnMouseLeave;
                        }
                    }
                }
                else if (_args.Action == NotifyCollectionChangedAction.Remove)
                {
                    if (_args.OldItems == null) return;
                    foreach (CheckBoxConnectedElement element in _args.OldItems)
                    {
                        if (element.TargetElement != null)
                        {
                            element.TargetElement.Tag = null;
                            element.TargetElement.MouseLeftButtonDown -= TargetElementOnMouseLeftButtonDown;
                            element.TargetElement.MouseEnter -= TargetElementOnMouseEnter;
                            element.TargetElement.MouseLeave -= TargetElementOnMouseLeave;
                        }
                    }
                }
            };

            foreach (var element in newElements)
            {
                if (element.TargetElement != null)
                {
                    element.TargetElement.Tag = checkBox;
                    element.TargetElement.MouseLeftButtonDown += TargetElementOnMouseLeftButtonDown;
                    element.TargetElement.MouseEnter += TargetElementOnMouseEnter;
                    element.TargetElement.MouseLeave += TargetElementOnMouseLeave;
                }
            }
        }

        private static void TargetElementOnMouseLeftButtonDown(object _sender, MouseButtonEventArgs _e)
        {
            var targetElement = _sender as FrameworkElement;
            if (targetElement == null)
                return;
            var cb = targetElement.Tag as CheckBox;
            if (cb == null)
                return;
            cb.IsChecked = !cb.IsChecked;
        }

        private static void TargetElementOnMouseEnter(object _sender, MouseEventArgs _e)
        {
            var targetElement = _sender as FrameworkElement;
            if (targetElement == null)
                return;

            var cb = targetElement.Tag as CheckBox;
            if (cb == null)
                return;

            // Enter 들어간 순간에 Disable 되어버리는 경우에 대한 예외처리는 없다.
            // Disable 안되는 개체만 써야할 듯..
            if (cb.IsEnabled)
            {
                cb.BorderBrush = BrushEx.FromHex(0x5593FFFF);
            }
        }

        private static void TargetElementOnMouseLeave(object _sender, MouseEventArgs _e)
        {
            var targetElement = _sender as FrameworkElement;
            if (targetElement == null)
                return;

            var cb = targetElement.Tag as CheckBox;
            if (cb == null)
                return;

            if (cb.IsEnabled)
            {
                cb.BorderBrush = BrushEx.FromHex(0x707070FF);
            }
        }
    }
}
