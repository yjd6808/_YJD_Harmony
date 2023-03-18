/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 4:18:51 PM
 *
 */

using SGToolsUI.Model.Main;
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

namespace SGToolsUI.Extension
{
    public static partial class PointEx
    {
        public static Point Zoom(this Point point, ZoomState zoomState) 
        {
            return new Point(point.X / zoomState.ZoomLevelX, point.Y / zoomState.ZoomLevelY);
        }

        public static Vector Zoom(this Vector vector, ZoomState zoomState) 
        {
            return new Vector(vector.X / zoomState.ZoomLevelX, vector.Y / zoomState.ZoomLevelY);
        }
    }
}
