/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System;

namespace SGToolsCommon.Customize.Control
{
    [Flags]
    public enum RTBTextDecoration
    {
        None          = 0,
        Bold          = 1 << 0,
        Italic        = 1 << 1,
        Underline     = 1 << 2,
        Strikethrough = 1 << 3,
    }
}
