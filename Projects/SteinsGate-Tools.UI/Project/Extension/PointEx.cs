/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 4:18:51 PM
 *
 */

using System.Windows;
using SGToolsCommon.Primitive;
using SGToolsUI.Model.Main;

namespace SGToolsUI.Extension
{
    public static partial class PointEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static IntPoint Zoom(this Point _point, ZoomState _zoomState)
        {
            return new IntPoint((int)(_point.X / _zoomState.ZoomLevelX), (int)(_point.Y / _zoomState.ZoomLevelY));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Vector Zoom(this Vector _vector, ZoomState _zoomState)
        {
            return new Vector(_vector.X / _zoomState.ZoomLevelX, _vector.Y / _zoomState.ZoomLevelY);
        }
    }
}
