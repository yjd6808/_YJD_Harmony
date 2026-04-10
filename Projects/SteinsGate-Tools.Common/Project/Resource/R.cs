/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 11:20:08 AM
 *
 */

using System;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Resources;

namespace SGToolsCommon.Resource
{
    // 폰트 파일명과 동일하게
    public enum FontType
    {
        DF,
        N2G,
        N2GB,
        N2GM,
        Max
    }

    public enum IconCommonType
    {
        Checked,
        Delete,
        Down,
        Empty,
        Grid,
        Music,
        NotUsable,
        Rect,
        Refresh,
        Reset,
        Select,
        Spark,
        Up,
        Position,
        Anchor,
        Unlock,
        Info,
        Backup,
        Pin,
        Unpin,
        Transparent,
        Album,
        Unclip,
        Link,
        Max
    }

    public class R
    {
        public const string ResourcePath = "pack://application:,,,/SGToolsCommon;component/Resource/";
        public const string IconPath = ResourcePath + "Icon/";
        public const string CursorPath = ResourcePath + "Cursor/";
        public const string FontPath = ResourcePath + "Font/";
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
        public const string IconCommonAnchorKey = IconPath + "common_anchor.ico";
        public const string IconCommonUnlockKey = IconPath + "common_unlock.ico";
        public const string IconCommonInfoKey = IconPath + "common_info.ico";
        public const string IconCommonBackupKey = IconPath + "common_backup.ico";
        public const string IconCommonPinKey = IconPath + "common_pin.ico";
        public const string IconCommonUnpinKey = IconPath + "common_unpin.ico";
        public const string IconCommonTransparentKey = IconPath + "common_transparent.ico";
        public const string IconCommonAlbumKey = IconPath + "common_album.ico";
        public const string IconCommonUnclipKey = IconPath + "common_unclip.ico";
        public const string IconCommonLinkKey = IconPath + "common_link.ico";

        public const string IconArrowTopLeftKey = IconPath + "arrow_topleft.ico";
        public const string IconArrowTopKey = IconPath + "arrow_top.ico";
        public const string IconArrowTopRightKey = IconPath + "arrow_topright.ico";
        public const string IconArrowRightKey = IconPath + "arrow_right.ico";
        public const string IconArrowBottomRightKey = IconPath + "arrow_bottomright.ico";
        public const string IconArrowBottomKey = IconPath + "arrow_bottom.ico";
        public const string IconArrowBottomLeftKey = IconPath + "arrow_bottomleft.ico";
        public const string IconArrowLeftKey = IconPath + "arrow_left.ico";

        public const string IconWinCloseKey = IconPath + "win_close.ico";
        public const string IconWinMaximizeKey = IconPath + "win_maximize.ico";
        public const string IconWinMinimizeKey = IconPath + "win_minimize.ico";

        public static readonly string FontDFKey = "gasinamuM";
        public static readonly string FontN2GKey = "NEXON Lv2 Gothic";
        public static readonly string FontN2GBKey = "NEXON Lv2 Gothic Bold";
        public static readonly string FontN2GMKey = "NEXON Lv2 Gothic Medium";

        private static readonly BitmapImage[] IconCommonMap = new BitmapImage[(int)IconCommonType.Max];
        private static readonly FontFamily[] FontCommonMap = new FontFamily[(int)FontType.Max];

        public static readonly Lazy<Cursor> DragAndDropCursor = new(() => ReadCursor(CursorPath + "drag_and_drop.ani"));

        //////////////////////////////////////////////////////////////////////////////////
        public static FontFamily GetFontFamily(FontType _type)
        {
            int index = (int)_type;
            if (FontCommonMap[index] == null)
                FontCommonMap[index] = new FontFamily(new Uri(FontPath), GetFontFamilyKey(_type));

            return FontCommonMap[index];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static BitmapImage GetIconCommon(IconCommonType _type)
        {
            int index = (int)_type;
            if (IconCommonMap[index] == null)
                IconCommonMap[index] = new BitmapImage(new Uri(GetIconCommonKey(_type)));

            return IconCommonMap[index];
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static Cursor ReadCursor(string _cursorPackPath)
        {
            StreamResourceInfo sri = Application.GetResourceStream(new Uri(_cursorPackPath)) ?? throw new Exception($"{_cursorPackPath} 리소스 로딩 실패");
            byte[] buffer = new byte[sri.Stream.Length];
            sri.Stream.Read(buffer, 0, buffer.Length);
            MemoryStream ms = new MemoryStream(buffer);
            return new Cursor(ms);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static string GetFontFamilyKey(FontType _type)
        {
            switch (_type)
            {
                case FontType.DF:       return FontDFKey;
                case FontType.N2G:      return FontN2GKey;
                case FontType.N2GB:     return FontN2GBKey;
                case FontType.N2GM:     return FontN2GMKey;
                default:
                    throw new ArgumentOutOfRangeException(nameof(_type), _type, null);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static string GetIconCommonKey(IconCommonType _type)
        {
            switch (_type)
            {
                case IconCommonType.Checked:        return IconCommonCheckedKey;
                case IconCommonType.Delete:         return IconCommonDeleteKey;
                case IconCommonType.Down:           return IconCommonDownKey;
                case IconCommonType.Empty:          return IconCommonEmptyKey;
                case IconCommonType.Grid:           return IconCommonGridKey;
                case IconCommonType.Music:          return IconCommonMusicKey;
                case IconCommonType.NotUsable:      return IconCommonNotUsableKey;
                case IconCommonType.Rect:           return IconCommonRectKey;
                case IconCommonType.Refresh:        return IconCommonRefreshKey;
                case IconCommonType.Reset:          return IconCommonResetKey;
                case IconCommonType.Select:         return IconCommonSelectKey;
                case IconCommonType.Spark:          return IconCommonSparkKey;
                case IconCommonType.Up:             return IconCommonUpKey;
                case IconCommonType.Position:       return IconCommonPositionKey;
                case IconCommonType.Anchor:         return IconCommonAnchorKey;
                case IconCommonType.Unlock:         return IconCommonUnlockKey;
                case IconCommonType.Info:           return IconCommonInfoKey;
                case IconCommonType.Backup:         return IconCommonBackupKey;
                case IconCommonType.Pin:            return IconCommonPinKey;
                case IconCommonType.Unpin:          return IconCommonUnpinKey;
                case IconCommonType.Transparent:    return IconCommonTransparentKey;
                case IconCommonType.Album:          return IconCommonAlbumKey;
                case IconCommonType.Unclip:         return IconCommonUnclipKey;
                case IconCommonType.Link:           return IconCommonLinkKey;
                default:  throw new ArgumentException("몽미");
            }
        }
    }
}
