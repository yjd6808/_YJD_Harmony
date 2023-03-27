/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:22 AM
 *
 */

using System;
using System.Collections.Generic;
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

        private SgaSpriteRect _spriteRect;
        private byte[] _data;
        private int _dataOffset;
        private int _dataLength;
        private bool _linearDodge;
        private SgaCompressMode _compressMode;


        public SgaCompressMode CompressMode => _compressMode;
        public override SgaSpriteRect SpriteRect => _spriteRect;
        public override Rect Rect => new (_spriteRect.X, _spriteRect.Y, _spriteRect.Width, _spriteRect.Height);
        public override Size Size => new(_spriteRect.Width, _spriteRect.Height);
        public override Size FrameSize => new(_spriteRect.FrameWidth, _spriteRect.FrameHeight);
        public int DataOffset => _dataOffset;
        public int DataLength => _dataLength;
        public override bool Loaded => _data != null;
        public override bool IsDummy => _spriteRect.Width * _spriteRect.Height <= 8;
        public override int Width => _spriteRect.Width;
        public override int Height => _spriteRect.Height;
        public override int X => _spriteRect.X;
        public override int Y => _spriteRect.Y;
        public override int FrameWidth => _spriteRect.Width;
        public override int FrameHeight => _spriteRect.Height;
        public override int TargetFrameIndex => FrameIndex;

        public bool IsLinearDodged
        {
            get => _linearDodge;
            set
            {
                if (_linearDodge == value)
                    return;

                if (!value)
                    Load();
                else
                    ApplyLinearDodge();

                _linearDodge = value;
            }
        }

        
        public override BitmapSource Source
        {
            get
            {
                if (!Loaded)
                    Load();

                if (_linearDodge)
                    ApplyLinearDodge();

                // 얕은 복사 좋아
                var source = BitmapSource.Create(Width, Height, 0, 0, PixelFormats.Bgra32, null, _data, 4 * Width);
                return source;
            }
        }

        public override System.Drawing.Bitmap Bitmap
        {
            get
            {
                if (!Loaded)
                    Load();

                // 너무나도 깊은 복사 조심
                return BitmapEx.FromArray(_data, Width, Height);
            }
        }

        // Xaml 초기화 전용
        public SgaSprite() : base(SgaSpriteType.Sprite, SgaColorFormat.Argb8888, null, 0)
        {
            _data = DummyTexture;
            _dataLength = _data.Length;
            _spriteRect = new SgaSpriteRect()
            {
                FrameHeight = 2,
                FrameWidth = 2,
                Width = 2,
                Height = 2,
                X = 0,
                Y = 0
            };
        }


        public SgaSprite(SgaSpriteRect spriteRect, SgaColorFormat colorFormat, SgaCompressMode compressedMode, int dataOffset, int dataLength,  SgaImage parent, int frameIndex) : base(SgaSpriteType.Sprite, colorFormat, parent, frameIndex)
        {
            _spriteRect = spriteRect;
            _compressMode = compressedMode;
            _dataLength = dataLength;
            _dataOffset = dataOffset;
            _data = null;
        }

        

        public override void Load()
        {
            if (_data != null && !_linearDodge)
                return;

            Stream stream = Parent.Parent.ReadStream;
            _data = new byte[_dataLength];
            stream.Seek(_dataOffset, SeekOrigin.Begin);
            stream.Read(_data, 0, _dataLength);

            Decompress();

            if (_linearDodge)
                ApplyLinearDodge(true);
        }

        public void ApplyLinearDodge(bool force = false)
        {
            if (!force && _linearDodge)
                return;

            if (!Loaded)
                Load();

            for (int i = 0; i < _data.Length; i += 4)
                BitmapEx.Bgra32LinearDodgePixel(_data.AsSpan(i, 4));

            _linearDodge = true;
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

        public override string ToString()
            => $"{FrameIndex}" + (IsLink ? $"링크({TargetFrameIndex})" : "");
    }
}
 