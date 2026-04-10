/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 10:19:42 AM
 */

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace SGToolsCommon.Extension
{
    public static class VisualEx
    {
        public class HitResult<TItem> where TItem : Control
        {
            public TItem Item { get; }

            //////////////////////////////////////////////////////////////////////////////////
            public HitResult(TItem _item) => Item = _item;
        }

        public class HitResultEx<TItem, TDataContext> : HitResult<TItem>
            where TItem : Control
            where TDataContext : class
        {
            public TDataContext DataContext { get; }

            //////////////////////////////////////////////////////////////////////////////////
            public HitResultEx(TItem _item, TDataContext _dataContext) : base(_item)
                => DataContext = _dataContext;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 복붙
        public static T FindParent<T>(this DependencyObject _child) where T : DependencyObject
        {
            DependencyObject parentObject = VisualTreeHelper.GetParent(_child);
            if (parentObject == null) return null;

            T parent = parentObject as T;
            if (parent != null)
                return parent;
            else
                return FindParent<T>(parentObject);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ForEachParent(this DependencyObject _child, Action<DependencyObject> _action)
        {
            DependencyObject parentObject = VisualTreeHelper.GetParent(_child);

            if (parentObject == null)
                return;

            _action(parentObject);
            parentObject.ForEachParent(_action);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static T FindChild<T>(this DependencyObject _depObj)
            where T : DependencyObject
        {
            if (_depObj == null) return null;

            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(_depObj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(_depObj, i);

                T result = (child as T) ?? FindChild<T>(child);
                if (result != null) return result;
            }
            return null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void PrintChildren(this DependencyObject _depObj)
        {
            if (_depObj == null) return;

            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(_depObj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(_depObj, i);
                Debug.WriteLine(child);
                PrintChildren(child);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Point GetOffsetIn(this Visual _depObj, Visual _relative)
        {
            // Visual 객체의 위치를 기준이 되는 UIElement 객체를 기준으로 변환합니다.
            GeneralTransform transform = _depObj.TransformToVisual(_relative);

            // 변환된 위치를 Point 객체로 변환합니다.
            return transform.Transform(new Point(0, 0));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Point GetOffsetInMonitor(this Visual _depObj)
        {
            Vector offset = VisualTreeHelper.GetOffset(_depObj);
            return _depObj.PointToScreen(new Point(offset.X, offset.Y));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static HitResultEx<TItem, TDataContext> HitTest<T, TItem, TDataContext>(this T _visual, Point _posOnVisual)
            where T : Visual
            where TItem : Control
            where TDataContext : class
        {
            HitTestResult hit = VisualTreeHelper.HitTest(_visual, _posOnVisual);

            if (hit.VisualHit == null)
                return null;

            TItem hitItem = hit.VisualHit.FindParent<TItem>();

            if (hitItem == null)
                return null;

            if (hitItem.DataContext is not TDataContext)
                throw new Exception($"선택한 {typeof(T).Namespace} 아이템의 데이터컨텍스트가 설정되어있지 않습니다.");

            TDataContext hitDataContext = hitItem.DataContext as TDataContext;
            return new HitResultEx<TItem, TDataContext>(hitItem, hitDataContext);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static HitResult<TItem> HitTest<T, TItem>(this T _visual, Point _posOnVisual)
            where T : Visual
            where TItem : Control
        {
            HitTestResult hit = VisualTreeHelper.HitTest(_visual, _posOnVisual);

            if (hit.VisualHit == null)
                return null;

            TItem hitItem = hit.VisualHit.FindParent<TItem>();

            if (hitItem == null)
                return null;

            return new HitResult<TItem>(hitItem);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 윈도우기준으로 visual의 위치,크기 정보를 얻는다.
        public static Rect GetRectOnWindow(this FrameworkElement _frameworkElement)
        {
            Window window = Window.GetWindow(_frameworkElement);
            Point visualOffset = _frameworkElement.TransformToAncestor(window).Transform(new Point(0, 0));
            Size visualSize = new Size(_frameworkElement.ActualWidth, _frameworkElement.ActualHeight);
            return new Rect(visualOffset, visualSize);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool ContainPoint(this FrameworkElement _frameworkElement, Point _p)
            => _frameworkElement.GetRectOnWindow().Contains(_p);

        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/2914495/wpf-how-to-programmatically-remove-focus-from-a-textbox
        public static void FocusClear(this FrameworkElement _element)
        {
            // Kill logical focus
            FocusManager.SetFocusedElement(FocusManager.GetFocusScope(_element), null);
            // Kill keyboard focus
            Keyboard.ClearFocus();
        }
    }
}
