/*
 * 생성일: 2/27/2023 10:19:42 AM
 */

using SGToolsCommon.Sga;
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

namespace SGToolsCommon.Extension
{
    public static class VisualTreeHelperEx
    {
        // 복붙
        public static T FindParent<T>(this DependencyObject child) where T : DependencyObject
        {
            //get parent item
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null) return null;

            //check if the parent matches the type we're looking for
            T parent = parentObject as T;
            if (parent != null)
                return parent;
            else
                return FindParent<T>(parentObject);
        }

        public static void ForEachParent(this DependencyObject child, Action<DependencyObject> action)
        {
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            if (parentObject == null) 
                return;

            action(parentObject);
            parentObject.ForEachParent(action);
        }

        public static T FindChild<T>(this DependencyObject depObj)
            where T : DependencyObject
        {
            if (depObj == null) return null;

            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(depObj); i++)
            {
                var child = VisualTreeHelper.GetChild(depObj, i);

                var result = (child as T) ?? FindChild<T>(child);
                if (result != null) return result;
            }
            return null;
        }

        public static Point GetOffsetIn(this Visual depObj, Visual relative)
        {
            // Visual 객체의 위치를 기준이 되는 UIElement 객체를 기준으로 변환합니다.
            GeneralTransform transform = depObj.TransformToVisual(relative);

            // 변환된 위치를 Point 객체로 변환합니다.
            return transform.Transform(new Point(0, 0));
        }

        public static Point GetOffsetInMonitor(this Visual depObj)
        {
            Vector offset = VisualTreeHelper.GetOffset(depObj);
            return depObj.PointToScreen(new Point(offset.X, offset.Y));
        }



        // 매번 만들기 귀찮아서 하나로 합침
        // ListBox를 예로들어서 ListBox내에서 마우스로 아무곳 찍으면
        // 해당위치에서 ListBoxItem과 ListBoxItem에 바인딩된 데이터 컨텍스트 가져오는 함수

        public class HitExResult<TItem, TDataContext>
            where TItem : Control
            where TDataContext : class
        {
            public HitExResult(TItem item, TDataContext dataContext)
            {
                Item = item;
                DataContext = dataContext;
            }

            public TItem Item { get; }
            public TDataContext DataContext { get; }
        }
        public static HitExResult<TItem, TDataContext> HitTest<T, TItem, TDataContext>(this T visual, Point posOnVisual)
            where T : Visual
            where TItem : Control
            where TDataContext : class
        {
            HitTestResult hit = VisualTreeHelper.HitTest(visual, posOnVisual);

            if (hit.VisualHit == null)
                return null;

            var hitItem = hit.VisualHit.FindParent<TItem>();

            if (hitItem == null)
                return null;

            if (hitItem.DataContext is not TDataContext)
                throw new Exception($"선택한 {typeof(T).Namespace} 아이템의 데이터컨텍스트가 설정되어있지 않습니다.");

            TDataContext hitDataContext = hitItem.DataContext as TDataContext;
            return new HitExResult<TItem, TDataContext>(hitItem, hitDataContext);
        }

      
    }
}
