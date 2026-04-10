/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:57:52 AM
 *
 */

namespace SGToolsCommon.Sga
{
    public struct SgaSpriteRect
    {
        public SgaSpriteRect() { }

        public SgaSpriteRect(int _x, int _y, int _width, int _height, int _frameWidth, int _frameHeight)
        {
            X = _x;
            Y = _y;
            Width = _width;
            Height = _height;
            FrameWidth = _frameWidth;
            FrameHeight = _frameHeight;
        }

        public int X { get; set; } = 0;
        public int Y { get; set; } = 0;
        public int Width { get; set; } = 0;
        public int Height { get; set; } = 0;
        public int FrameWidth { get; set; } = 0;
        public int FrameHeight { get; set; } = 0;
    }
}
