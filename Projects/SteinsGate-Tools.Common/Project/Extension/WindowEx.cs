// 작성자 : 윤정도
// 1~2년전에 만든 윈도우 관련 처리

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using Vanara.PInvoke;
using static Vanara.PInvoke.User32;

namespace SGToolsCommon.Extension
{

    public static class WindowEx
    {
        /*
         * 프로세스, 작업표시줄 주황표시 영구적으로 하기
         * https://stackoverflow.com/questions/19831540/flash-external-window
         *
         * FLASHWINFO 구조체는 FlashWindowEx 함수에서 사용되는 구조체입니다.이 구조체는 윈도우를 깜박이게 하여 알림을 주기 위한 정보를 제공합니다.구조체의 필드는 다음과 같습니다.
         * cbSize: 구조체의 크기를 나타냅니다.이 필드는 구조체를 초기화할 때 sizeof(FLASHWINFO) 와 같은 값을 할당해야 합니다.
         * hwnd: 깜박이게 할 윈도우의 핸들입니다.
         * dwFlags: 깜박이기의 옵션을 지정합니다.다음과 같은 값들을 사용할 수 있습니다.
         *
         * FLASHW_CAPTION: 윈도우의 캡션 바를 깜박입니다.
         * FLASHW_TRAY: 작업 표시줄에 있는 아이콘을 깜박입니다.
         * FLASHW_ALL: 윈도우와 작업 표시줄 아이콘을 모두 깜박입니다.
         * FLASHW_TIMER: 윈도우를 깜박이게 합니다.이때, dwTimeout 필드에 설정된 시간 간격으로 깜박입니다.
         * FLASHW_TIMERNOFG: 윈도우를 깜박이게 하되, 해당 윈도우가 Foreground가 아닌 경우에만 깜박입니다.
         * uCount: 깜박이기의 횟수입니다. 0이면 무한히 깜박입니다.
         *
         * dwTimeout: 깜박이기 간격의 시간을 밀리초 단위로 지정합니다.
         * 위와 같은 필드들을 사용하여 깜박이기의 옵션을 설정할 수 있습니다.깜박이기가 완료된 후, FlashWindowEx 함수는 TRUE를 반환합니다.
         */

        public static void Flash(IntPtr handle)
        {
            FLASHWINFO fInfo = new FLASHWINFO();
            fInfo.cbSize = Convert.ToUInt32(Marshal.SizeOf(fInfo));
            fInfo.dwFlags = FLASHW.FLASHW_TRAY | FLASHW.FLASHW_TIMERNOFG;
            fInfo.dwTimeout = 3000; 
            fInfo.hwnd = handle;
            fInfo.uCount = 0;
            FlashWindowEx(fInfo);
        }

        public static IntPtr Handle(this Window window)
            => new WindowInteropHelper(window).Handle;

        // 해당 윈도우가 포그라운드 윈도우인지 체크
        // https://stackoverflow.com/questions/7162834/determine-if-current-application-is-activated-has-focus
        public static bool IsMainWindowForeground()
            => Application.Current?.MainWindow?.Handle() == User32.GetForegroundWindow();

        public static bool IsApplicationForeground()
        {

            var activatedHandle = User32.GetForegroundWindow();
            if (activatedHandle == IntPtr.Zero)
                return false;       // No window is currently activated

            int procId = Process.GetCurrentProcess().Id;
            User32.GetWindowThreadProcessId(activatedHandle, out uint activeProcId);
            return activeProcId == procId;
        }

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
