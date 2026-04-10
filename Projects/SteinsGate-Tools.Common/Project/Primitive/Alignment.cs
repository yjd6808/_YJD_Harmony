/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 9:08:25 PM
 *
 */

using System;

namespace SGToolsCommon.Primitive
{
    public enum VAlignment
    {
        Top,
        Center,
        Bottom
    }

    public enum HAlignment
    {
        Left,
        Center,
        Right
    }

    public enum Positioning
    {
        TopLeft,
        Top,
        TopRight,
        Left,
        Center,
        Right,
        BottomLeft,
        Bottom,
        BottomRight
    }

    public static class Alignment
    {
        public static readonly ValueTuple<VAlignment, HAlignment>[] PositioningToAlignMap = new[]
        {
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Top, HAlignment.Left),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Top, HAlignment.Center),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Top, HAlignment.Right),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Center, HAlignment.Left),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Center, HAlignment.Center),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Center, HAlignment.Right),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Bottom, HAlignment.Left),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Bottom, HAlignment.Center),
            new ValueTuple<VAlignment, HAlignment>(VAlignment.Bottom, HAlignment.Right)
        };
    }
}
