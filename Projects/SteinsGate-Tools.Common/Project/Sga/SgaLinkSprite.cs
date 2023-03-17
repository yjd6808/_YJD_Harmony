/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:15 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
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
using SGToolsCommon.Resource;
using Size = System.Windows.Size;

namespace SGToolsCommon.Sga
{
    public class SgaLinkSprite : SgaSpriteAbstract
    {
        public SgaLinkSprite(SgaImage parent, int targetFrameIndex, int frameIndex) : base(SgaSpriteType.LinkSprite, SgaColorFormat.Link, parent, frameIndex)
        {
            TargetFrameIndex = targetFrameIndex;  
        }

        
        public override bool IsDummy => false;
        public override bool Loaded => true;
        public override SgaSpriteRect SpriteRect => new(1, 1, 1, 1, 1, 1);
        public override Rect Rect => new(1, 1, 1, 1);
        public override Size Size => new(1, 1);
        public override Size FrameSize => new(1, 1);
        public override int Width => 1;
        public override int Height => 1;
        public override int X => 1;
        public override int Y => 1;
        public override int FrameWidth => 1;
        public override int FrameHeight => 1;
        public override int TargetFrameIndex { get; }
        private static Bitmap BitmapInstance = LinkBitmap();
        public override BitmapSource Source => R.GetIconCommon(IconCommonType.Link);

        public override Bitmap Bitmap => BitmapInstance;

        private static Bitmap LinkBitmap()
        {
            BitmapImage image = R.GetIconCommon(IconCommonType.Link); 
            MemoryStream memoryStream = new MemoryStream();
            BitmapEncoder encoder = new BmpBitmapEncoder();
            encoder.Frames.Add(BitmapFrame.Create(image));
            encoder.Save(memoryStream);

            // MemoryStream으로부터 Bitmap 생성
            Bitmap bitmap = new Bitmap(memoryStream);
            return bitmap;
        }

        public override void Load()
        {
        }

        public override void Unload()
        {
        }
    }
}
