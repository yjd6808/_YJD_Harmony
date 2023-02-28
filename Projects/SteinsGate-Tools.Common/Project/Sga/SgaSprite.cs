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
using System.Printing.IndexedProperties;
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
using SGToolsCommon.ThirdParty;

namespace SGToolsCommon.Sga
{
    public class SgaSprite : SgaSpriteAbstract
    {
        private static byte[] DummyTexture = new byte[] {
            // RGBA8888
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF
        };

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
        public override int Width => _rect.Width;
        public override int Height => _rect.Height;
        public override int X => _rect.X;
        public override int Y => _rect.Y;
        public override int FrameWidth => _rect.Width;
        public override int FrameHeight => _rect.Height;
        public override int TargetFrameIndex => FrameIndex;

        public BitmapSource Source
        {
            get
            {
                if (!Loaded)
                    Load();

                return BitmapSource.Create(Width, Height, 0, 0, PixelFormats.Bgra32, null, _data, 4 * Width);
            }
        }

        public Bitmap Bitmap
        {
            get
            {
                if (!Loaded)
                    Load();

                return BitmapEx.FromArray(_data, Width, Height);
            }
        }

        // Xaml 초기화 전용
        public SgaSprite() : base(SgaSpriteType.Sprite, SgaColorFormat.Argb8888, null, 0)
        {
            _data = DummyTexture;
            _dataLength = _data.Length;
            _rect = new SgaSpriteRect()
            {
                FrameHeight = 2,
                FrameWidth = 2,
                Width = 2,
                Height = 2,
                X = 0,
                Y = 0
            };
        }


        public SgaSprite(SgaSpriteRect rect, SgaColorFormat colorFormat, SgaCompressMode compressedMode, int dataOffset, int dataLength,  SgaImage parent, int frameIndex) : base(SgaSpriteType.Sprite, colorFormat, parent, frameIndex)
        {
            _rect = rect;
            _compressMode = compressedMode;
            _dataLength = dataLength;
            _dataOffset = dataOffset;
            _data = null;
        }



        public override void Load()
        {
            Stream stream = Parent.Parent.ReadStream;
            _data = new byte[_dataLength];
            stream.Seek(_dataOffset, SeekOrigin.Begin);
            stream.Read(_data, 0, _dataLength);

            Decompress();
        }

        public override void Unload()
        {
            _data = null;
        }

        private void Decompress()
        {
            int iDecompressedSize = Width * Height * (ColorFormat == SgaColorFormat.Argb8888 ? 4 : 2);

            byte[] decompressed = null;
            byte[] readData = _data;

            if (CompressMode == SgaCompressMode.Zlib)
            {
                decompressed = Zlib.Decompress(_data, iDecompressedSize);
                readData = decompressed;
            }

            if (ColorFormat == SgaColorFormat.Argb8888)
            {
                _data = decompressed == null ? readData : decompressed;
                return;
            }
                

            int bit32Size = iDecompressedSize * 2;
            byte[] bit32Data = new byte[bit32Size];
            byte[] readBytes = new byte[2];

            for (int i = 0, j = 0; i < bit32Size; i += 4, j += 2)
            {
                byte a = 0;
                byte r = 0;
                byte g = 0;
                byte b = 0;

                readBytes[0] = readData[j];
                readBytes[1] = readData[j + 1];

                switch (ColorFormat)
                {
                    case SgaColorFormat.Argb1555:
                        a = (byte)(readBytes[1] >> 7);
                        r = (byte)((readBytes[1] >> 2) & 0x1f);
                        g = (byte)((readBytes[0] >> 5) | ((readBytes[1] & 3) << 3));
                        b = (byte)(readBytes[0] & 0x1f);
                        a = (byte)(a * 0xff);
                        r = (byte)((r << 3) | (r >> 2));
                        g = (byte)((g << 3) | (g >> 2));
                        b = (byte)((b << 3) | (b >> 2));
                        break;
                    case SgaColorFormat.Argb4444:
                        a = (byte)(readBytes[1] & 0xf0);
                        r = (byte)((readBytes[1] & 0xf) << 4);
                        g = (byte)(readBytes[0] & 0xf0);
                        b = (byte)((readBytes[0] & 0xf) << 4);
                        break;
                }

                bit32Data[i] = b;
                bit32Data[i + 1] = g;
                bit32Data[i + 2] = r;
                bit32Data[i + 3] = a;
            }
            _data = bit32Data;
        }
    }
}
 