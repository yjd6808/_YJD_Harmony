/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:57:52 AM
 *
 */

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

namespace SGToolsCommon.Sga
{
    public struct SgaSpriteRect
    {
        public SgaSpriteRect() {}

        public SgaSpriteRect(int x, int y, int width, int height, int frameWidth, int frameHeight)
        {
            X = x;
            Y = y;
            Width = width; 
            Height = height;
            FrameWidth = frameWidth;
            FrameHeight = frameHeight;
        }

        public int X { get; set; } = 0;
        public int Y { get; set; } = 0;
        public int Width { get; set; } = 0;
        public int Height { get; set; } = 0;
        public int FrameWidth { get; set; } = 0;
        public int FrameHeight {  get; set; } = 0;
    }
}
