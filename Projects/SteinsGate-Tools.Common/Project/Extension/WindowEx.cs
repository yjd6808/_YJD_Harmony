// 작성자 : 윤정도
// 1~2년전에 만든 윈도우 관련 처리

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace SGToolsCommon.Extension
{

    public static class WindowEx
    {
        public static void MoveTo(this Window window, Point point, bool allowOver = true)
        {
            window.Left = point.X;
            window.Top = point.Y;
        }

        public static Point GetCenterPosition(this Window window)
        {
            return new global::System.Windows.Point(
                window.Left + window.Width / 2,
                window.Top + +window.Height / 2);
        }

        // window 기준으로 가장 가까운 디스플레이 바운드를 반환한다.
        public static Rect ClosestDisplayRect(this Window window)
        {
            List<Rect> rectList = DisplayEx.GetDisplayRectList();
            Point windowCenterPosition = WindowEx.GetCenterPosition(window);

            // 현재 윈도우가 포함된 디스플레이 가져오기
            Rect closestDisplayRect = rectList.Where(x => x.Contains(windowCenterPosition)).FirstOrDefault();

            // 포함 안되있을 경우
            if (closestDisplayRect == default(Rect))
            {
                // 디스플레이중 메인 윈도우와 가장 가까운 디스플레이 가져오기
                rectList.Sort(new DisplayDistanceComparer(windowCenterPosition));
                closestDisplayRect = rectList[0];
            }

            return closestDisplayRect;
        }

        // window를 현재 window에서 가장 가까운 디스플레이의 중앙에 위치하도록 한다.
        public static void MoveToClosestDisplayCenter(this Window window)
        {
            Size closestDisplayRect = ClosestDisplayRect(window).Size;

            float width = (float)closestDisplayRect.Width;
            float height = (float)closestDisplayRect.Height;

            window.Left = width / 2 - (double.IsNaN(window.Width) ? 0 : window.Width / 2);
            window.Top = height / 2 - (double.IsNaN(window.Height) ? 0 : window.Height / 2);
        }


        // window를 현재 baseWindow의 중앙에 위치하도록 한다.
        public static void MoveToCenterFrom(this Window window, Window baseWindow)
        {
            Point baseCenter = GetCenterPosition(baseWindow);

            window.Left = baseCenter.X - window.Width / 2;
            window.Top = baseCenter.Y - window.Height / 2;
        }

        // baseWindow 윈도우 기준으로 가장 가까운 디스플레이를 찾고
        // adjustWindow 윈도우를 가장 가까운 디스플레이를 벗어나지 않는 선에서 위치를 세팅한다.
        public static void AdjustPosition(Window baseWindow, Window adjustWindow, int padding)
        {
            // 현재 윈도우가 포함된 디스플레이 가져오기
            Rect closestDisplayRect = ClosestDisplayRect(baseWindow);

            double bottom = adjustWindow.Top + adjustWindow.Height;
            double right = adjustWindow.Left + adjustWindow.Width;

            if (adjustWindow.Top <= closestDisplayRect.Top)
                adjustWindow.Top = closestDisplayRect.Top + padding;
            if (bottom >= closestDisplayRect.Bottom)
                adjustWindow.Top = closestDisplayRect.Bottom - (padding + adjustWindow.Height);
            if (adjustWindow.Left <= closestDisplayRect.Left)
                adjustWindow.Left = closestDisplayRect.Left + padding;
            if (right >= closestDisplayRect.Right)
                adjustWindow.Left = closestDisplayRect.Right - (padding + adjustWindow.Width);
        }

        // 윈도우 크기가 디스플레이를 벗어나게 큰 경우 기존 윈도우 크기의 비율을 유지하며 크기를 줄여준다.
        // 이때 디스플레이는 window를 기준으로 가장 가까운 디스플레이의 너비와 높이를 기준으로 한다.
        // Adjust 된 경우 true 안된 경우 false
        // 하.. 높이가 디스플레이 높이보다 큰 경우 그냥 고정되어버리네 크게 쓸모 없을 듯;
        public static bool AdjustSizeOnScreen(Window window, int padding)
        {
            if (double.IsNaN(window.Width) || double.IsNaN(window.Height))
                throw new Exception("윈도우의 너비와 높이가 결정되어야만 사용가능합니다.");

            Rect closestDisplayRect = ClosestDisplayRect(window);

            double paddedDisplayWidth = closestDisplayRect.Width + padding;
            double paddedDisplayHeight = closestDisplayRect.Height + padding;

            if (window.Width > paddedDisplayWidth && window.Height > paddedDisplayHeight)
            {
                // 윈도우 너비, 높이가 디스플레이 너비, 높이를 모두 초과하는 경우
                // 보통 디스플레이가 세로길이가 더 짧으니 짧은쪽기준으로 맞춰주자.

                // 먼저 기존 너비와 높이 비율을 구한다.
                double windowSizeRatio = window.Width / window.Height;

                window.Height = paddedDisplayHeight;
                window.Width = window.Height * windowSizeRatio;

                window.Top = padding;
                window.Left = padding;
                return true;
            }
            else if (window.Width > paddedDisplayWidth)
            {
                window.Height = window.Height * paddedDisplayWidth / window.Width;
                window.Width = paddedDisplayWidth;
                window.Left = padding;
                return true;
            }
            else if (window.Height > paddedDisplayHeight)
            {
                window.Width = window.Width * paddedDisplayHeight / window.Height;
                window.Height = paddedDisplayHeight;
                window.Top = padding;
                return true;
            }
            else
            {
                return false;
            }
        }
    }


    public class DisplayDistanceComparer : IComparer<Rect>
    {
        private Point _position;

        public DisplayDistanceComparer(Point p)
        {
            _position = p;
        }

        private double Distance(Rect rect)
        {
            Point displayCenterLocation = new global::System.Windows.Point(rect.Location.X + rect.Width / 2, rect.Location.Y + rect.Height / 2);

            return Math.Sqrt(
                Math.Pow(_position.X - displayCenterLocation.X, 2) +
                Math.Pow(_position.Y - displayCenterLocation.Y, 2)
            );
        }

        public int Compare(Rect x, Rect y)
        {
            return Distance(x).CompareTo(Distance(y));
        }
    }
}
