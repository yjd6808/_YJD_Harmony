/*
 * 작성자: 윤정도
 * 생성일: 10/1/2024 4:59:24 PM
 * TargetElement의 Visibility변화에 따라서 RowDefinition의 Height를 동적으로 변경한다.
 * 아래처럼 xaml로 처리할 수 있지만 다른 곳에서 재활용하기에는 너무 많은 line을 차지함
 * <RowDefinition.Style>
 *     <Style>
 *         <Style.Triggers>
 *             <DataTrigger Binding="{Binding ElementName=RememberedComPathListView, Path=Visibility}" Value="Collapsed">
 *                 <Setter Property="RowDefinition.Height" Value="0"/>
 *             </DataTrigger>
 *             <DataTrigger Binding="{Binding ElementName=RememberedComPathListView, Path=Visibility}" Value="Visible">
 *                 <Setter Property="RowDefinition.Height" Value="*"/>
 *             </DataTrigger>
 *             <DataTrigger Binding="{Binding ElementName=RememberedComPathListView, Path=Visibility}" Value="Hidden">
 *                 <Setter Property="RowDefinition.Height" Value="0"/>
 *             </DataTrigger>
 *         </Style.Triggers>
 *     </Style>
 * </RowDefinition.Style>
 */

using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;

namespace SGToolsCommon.Customize.Behavior
{
    public class RowDefinitionElementVisibilityBindingBehavior
    {
        public static readonly DependencyProperty TargetElementProperty =
            DependencyProperty.RegisterAttached(
                "TargetElement", typeof(FrameworkElement), typeof(RowDefinitionElementVisibilityBindingBehavior), new PropertyMetadata(null, OnTargetElementChanged));

        public static readonly DependencyProperty CollapsedHeightProperty =
            DependencyProperty.RegisterAttached(
                "CollapsedHeight", typeof(GridLength), typeof(RowDefinitionElementVisibilityBindingBehavior), new PropertyMetadata(new GridLength(0)));

        public static readonly DependencyProperty VisibleHeightProperty =
            DependencyProperty.RegisterAttached(
                "VisibleHeight", typeof(GridLength), typeof(RowDefinitionElementVisibilityBindingBehavior), new PropertyMetadata(new GridLength(1, GridUnitType.Star)));

      
        public static FrameworkElement GetTargetElement(DependencyObject _obj)
        {
            return (FrameworkElement)_obj.GetValue(TargetElementProperty);
        }

        public static void SetTargetElement(DependencyObject _obj, FrameworkElement _value)
        {
            _obj.SetValue(TargetElementProperty, _value);
        }

        public static GridLength GetCollapsedHeight(DependencyObject _obj)
        {
            return (GridLength)_obj.GetValue(CollapsedHeightProperty);
        }

        public static void SetCollapsedHeight(DependencyObject _obj, GridLength _value)
        {
            _obj.SetValue(CollapsedHeightProperty, _value);
        }

        public static GridLength GetVisibleHeight(DependencyObject _obj)
        {
            return (GridLength)_obj.GetValue(VisibleHeightProperty);
        }

        public static void SetVisibleHeight(DependencyObject _obj, GridLength _value)
        {
            _obj.SetValue(VisibleHeightProperty, _value);
        }

        private static void OnTargetElementChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            var rowDefinition = _d as RowDefinition;
            var oldElement = _e.OldValue as FrameworkElement;
            var newElement = _e.NewValue as FrameworkElement;

            if (rowDefinition == null)
                return;

            if (oldElement != null)
            {
                oldElement.IsVisibleChanged -= OnTargetElementIsVisibleChanged;
                oldElement.Tag = null;
            }

            if (newElement != null)
            {
                Debug.Assert(newElement.Tag == null, "어.. Tag는 RowDefinition 레퍼런스 저장용도로 씁니다.. ㅠ");
                newElement.IsVisibleChanged += OnTargetElementIsVisibleChanged;
                newElement.Tag = rowDefinition;
                UpdateRowHeight(rowDefinition, newElement.Visibility);
            }
        }

        private static void OnTargetElementIsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            var targetElement = sender as FrameworkElement;
            var rowDefinition = targetElement?.Tag as RowDefinition;

            if (rowDefinition != null && targetElement != null)
            {
                UpdateRowHeight(rowDefinition, targetElement.Visibility);
            }
        }

        private static void UpdateRowHeight(RowDefinition _rowDefinition, Visibility _visibility)
        {
            if (_visibility == Visibility.Collapsed || _visibility == Visibility.Hidden)
            {
                _rowDefinition.Height = GetCollapsedHeight(_rowDefinition);
            }
            else
            {
                _rowDefinition.Height = GetVisibleHeight(_rowDefinition);
            }
        }
    }
}
