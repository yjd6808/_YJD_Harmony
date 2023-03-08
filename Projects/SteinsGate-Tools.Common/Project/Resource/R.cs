/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 11:20:08 AM
 *
 */

using System;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Windows.Resources;

namespace SGToolsCommon.Resource
{
    public class R
    {
        public const string ResourcePath = "pack://application:,,,/SGToolsCommon;component/Resource/";
        public const string IconPath = ResourcePath + "Icon/";
        public const string CursorPath = ResourcePath + "Cursor/";
        public const string ImagePath = ResourcePath + "Image/";

        public const string IconCommonCheckedKey = IconPath + "common_checked.ico";
        public const string IconCommonDeleteKey = IconPath + "common_delete.ico";
        public const string IconCommonDownKey = IconPath + "common_down.ico";
        public const string IconCommonEmptyKey = IconPath + "common_empty.ico";
        public const string IconCommonGridKey = IconPath + "common_grid.ico";
        public const string IconCommonMusicKey = IconPath + "common_music.ico";
        public const string IconCommonNotUsableKey = IconPath + "common_not_usable.ico";
        public const string IconCommonRectKey = IconPath + "common_rect.ico";
        public const string IconCommonRefreshKey = IconPath + "common_refresh.ico";
        public const string IconCommonResetKey = IconPath + "common_reset.ico";
        public const string IconCommonSelectKey = IconPath + "common_select.ico";
        public const string IconCommonSparkKey = IconPath + "common_spark.ico";
        public const string IconCommonUpKey = IconPath + "common_up.ico";
        public const string IconCommonPositionKey = IconPath + "common_position.ico";

        public const string IconWinCloseKey = IconPath + "win_close.ico";
        public const string IconWinMaximizeKey = IconPath + "win_maximize.ico";
        public const string IconWinMinimizeKey = IconPath + "win_minimize.ico";

        public static readonly Lazy<Cursor> DragAndDropCursor = new(() => ReadCursor(CursorPath + "drag_and_drop.ani"));

        private static Cursor ReadCursor(string cursorPackPath)
        {
            StreamResourceInfo sri = Application.GetResourceStream(new Uri(cursorPackPath)) ?? throw new Exception($"{cursorPackPath} 리소스 로딩 실패");
            byte[] buffer = new byte[sri.Stream.Length];
            sri.Stream.Read(buffer, 0, buffer.Length);
            MemoryStream ms = new MemoryStream(buffer);
            return new Cursor(ms);
        }
    }
}
