/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:22 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Sga
{
    public class SgaSprite : SgaSpriteAbstract
    {
        
        private SgaSpriteRect _rect;
        private byte[] _data;
        private int _dataOffset;
        private int _dataLength;
        private SgaCompressMode _compressMode;


        public SgaCompressMode CompressMode => _compressMode;
        public SgaSpriteRect Rect => _rect;
        public int DataOffset => _dataOffset;
        public int DataLength => _dataLength;
        public override bool Loaded => _data != null;
        public override bool IsDummy => _rect.Width * _rect.Height <= 8;
        public int Width => _rect.Width;
        public int Height => _rect.Height;
        public int X => _rect.X;
        public int Y => _rect.Y;
        public int FrameWidth => _rect.Width;
        public int FrameHeight => _rect.Height;

        public BitmapSource Source =>
            BitmapSource.Create(Width, Height, 0, 0, PixelFormats.Bgra32, null, _data, 4 * Width);
        public Bitmap Bitmap =>
            BitmapEx.FromArray(_data, Width, Height);



        public SgaSprite(SgaSpriteRect rect, int dataOffset, int dataLength,  SgaColorFormat colorFormat, SgaImage parent, int frameIndex) : base(SgaSpriteType.Sprite, colorFormat, parent, frameIndex)
        {
            _rect = rect;
            _dataLength = dataLength;
            _dataOffset = dataOffset;
            _data = null;
        }



        public override void Load()
        {
            Stream stream = Parent.Parent.ReadStream;
            byte[] _data = new byte[_dataLength];
            stream.Seek(_dataOffset, SeekOrigin.Begin);
            stream.Read(_data, 0, _dataLength);

            //if (CompressMode == SgaCompressMode.Zlib)
                //data = Zlib.Decompress(data, size);

            //data = Bitmaps.ConvertTo32Bits(data, type);
        }

        public override void Unload()
        {
            _data = null;
        }

        private void Decompress()
        {


        }
    }
}
