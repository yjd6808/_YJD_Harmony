/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 10:19:42 AM
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
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
        public static T? FindParent<T>(this DependencyObject _child) where T : DependencyObject
        {
            DependencyObject parentObject = VisualTreeHelper.GetParent(_child);
            if (parentObject == null) return null;

            T? parent = parentObject as T;
            if (parent != null)
                return parent;
            else
                return FindParent<T>(parentObject);
        }

        public static T? FindParentFrameworkElement<T>(this FrameworkElement _child) where T : FrameworkElement
        {
            var fe = _child.Parent as FrameworkElement;
            if (fe == null)
                return null;

            if (fe is T parent)
                return parent;

            return FindParentFrameworkElement<T>(fe);
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
        public static T? FindChild<T>(this DependencyObject _depObj)
            where T : DependencyObject
        {
            if (_depObj == null) return null;

            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(_depObj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(_depObj, i);

                T? result = (child as T) ?? FindChild<T>(child);
                if (result != null) return result;
            }
            return null;
        }

        public static T? FindChild<T>(this FrameworkElement _frameworkObject, string _childName) where T : FrameworkElement
        {
            int childrenCount = VisualTreeHelper.GetChildrenCount(_frameworkObject);
            for (int i = 0; i < childrenCount; i++)
            {
                var child = VisualTreeHelper.GetChild(_frameworkObject, i) as FrameworkElement;
                if (child == null) continue;
                if (child is T result)
                {
                    if (result.Name == _childName)
                        return result;
                }
                return FindChild<T>(child, _childName);
            }
            return null;
        }

        public static IEnumerable<T> FindChildren<T>(this DependencyObject _depObj) where T : DependencyObject
        {
            if (_depObj == null) yield return (T)Enumerable.Empty<T>();
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(_depObj); i++)
            {
                DependencyObject ithChild = VisualTreeHelper.GetChild(_depObj, i);
                if (ithChild == null) continue;
                if (ithChild is T t) yield return t;
                foreach (T childOfChild in FindChildren<T>(ithChild)) yield return childOfChild;
            }
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
        public static HitResultEx<TItem, TDataContext>? HitTest<T, TItem, TDataContext>(this T _visual, Point _posOnVisual)
            where T : Visual
            where TItem : Control
            where TDataContext : class
        {
            HitTestResult hit = VisualTreeHelper.HitTest(_visual, _posOnVisual);

            if (hit.VisualHit == null)
                return null;

            TItem? hitItem = hit.VisualHit.FindParent<TItem>();

            if (hitItem == null)
                return null;

            if (hitItem.DataContext is not TDataContext)
                return null;

            TDataContext? hitDataContext = hitItem.DataContext as TDataContext;
            return new HitResultEx<TItem, TDataContext>(hitItem, hitDataContext!);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static HitResult<TItem>? HitTest<T, TItem>(this T _visual, Point _posOnVisual)
            where T : Visual
            where TItem : Control
        {
            HitTestResult hit = VisualTreeHelper.HitTest(_visual, _posOnVisual);

            if (hit.VisualHit == null)
                return null;

            TItem? hitItem = hit.VisualHit.FindParent<TItem>();

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

        // UIElement로부터 스크롤뷰어 얻기
        // @참고: https://stackoverflow.com/questions/41132649/get-datagrids-scrollviewer
        public static ScrollViewer? GetScrollViewerCommon(this UIElement _element)
        {
            if (_element == null)
                return null;

            ScrollViewer? retour = null;
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(_element) && retour == null; i++)
            {
                if (VisualTreeHelper.GetChild(_element, i) is ScrollViewer)
                {
                    retour = (ScrollViewer)(VisualTreeHelper.GetChild(_element, i));
                }
                else
                {
                    UIElement? uiElement = VisualTreeHelper.GetChild(_element, i) as UIElement;
                    if (uiElement == null)
                        continue;

                    retour = GetScrollViewerCommon(uiElement);
                }
            }
            return retour;
        }

        // 스크롤뷰어로부터 스크롤바 얻기
        // @참고: https://stackoverflow.com/questions/7164439/wpf-how-to-extract-scrollbar-from-scrollviewer-programmatically
        public static ScrollBar? GetScrollBar(this ScrollViewer _element)
        {
            return _element.Template.FindName("PART_VerticalScrollBar", _element) as ScrollBar;
        }
    }
}
