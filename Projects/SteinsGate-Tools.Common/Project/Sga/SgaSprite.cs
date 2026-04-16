/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:22 AM
 *
 */

using System;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

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

        private SgaSpriteRect spriteRect_;
        private byte[]? data_;
        private int dataOffset_;
        private int dataLength_;
        private bool linearDodge_;
        private SgaCompressMode compressMode_;

        public SgaCompressMode CompressMode => compressMode_;
        public override SgaSpriteRect SpriteRect => spriteRect_;
        public override Rect Rect => new(spriteRect_.X, spriteRect_.Y, spriteRect_.Width, spriteRect_.Height);
        public override Size Size => new(spriteRect_.Width, spriteRect_.Height);
        public override Size FrameSize => new(spriteRect_.FrameWidth, spriteRect_.FrameHeight);
        public int DataOffset => dataOffset_;
        public int DataLength => dataLength_;
        public override bool Loaded => data_ != null;
        public override bool IsDummy => spriteRect_.Width * spriteRect_.Height <= 8;
        public override int Width => spriteRect_.Width;
        public override int Height => spriteRect_.Height;
        public override int X => spriteRect_.X;
        public override int Y => spriteRect_.Y;
        public override int FrameWidth => spriteRect_.Width;
        public override int FrameHeight => spriteRect_.Height;
        public override int TargetFrameIndex => FrameIndex;

        public bool IsLinearDodged
        {
            get => linearDodge_;
            set
            {
                if (linearDodge_ == value)
                    return;

                if (!value)
                {
                    linearDodge_ = false;
                    Unload();
                    Load();
                }
                else
                {
                    ApplyLinearDodge();
                }
            }
        }

        public override BitmapSource Source
        {
            get
            {
                if (!Loaded)
                    Load();

                if (linearDodge_)
                    ApplyLinearDodge();

                // 얕은 복사 좋아
                BitmapSource source = BitmapSource.Create(Width, Height, 0, 0, PixelFormats.Bgra32, null, data_!, 4 * Width);
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
                return BitmapEx.FromArray(data_!, Width, Height);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Xaml 초기화 전용
        public SgaSprite() : base(SgaSpriteType.Sprite, SgaColorFormat.Argb8888, null, 0)
        {
            data_ = DummyTexture;
            dataLength_ = data_.Length;
            spriteRect_ = new SgaSpriteRect()
            {
                FrameHeight = 2,
                FrameWidth = 2,
                Width = 2,
                Height = 2,
                X = 0,
                Y = 0
            };
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSprite(SgaSpriteRect _spriteRect, SgaColorFormat _colorFormat, SgaCompressMode _compressedMode, int _dataOffset, int _dataLength, SgaImage _parent, int _frameIndex)
            : base(SgaSpriteType.Sprite, _colorFormat, _parent, _frameIndex)
        {
            spriteRect_ = _spriteRect;
            compressMode_ = _compressedMode;
            dataLength_ = _dataLength;
            dataOffset_ = _dataOffset;
            data_ = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Load()
        {
            if (data_ != null && !linearDodge_)
                return;

            Stream stream = Parent!.Parent.ReadStream;
            data_ = new byte[dataLength_];
            stream.Seek(dataOffset_, SeekOrigin.Begin);
            stream.Read(data_, 0, dataLength_);

            Decompress();

            if (linearDodge_)
                ApplyLinearDodge(true);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ApplyLinearDodge(bool _force = false)
        {
            if (!_force && linearDodge_)
                return;

            if (!Loaded)
                Load();

            for (int i = 0; i < data_!.Length; i += 4)
                BitmapEx.Bgra32LinearDodgePixel(data_!.AsSpan(i, 4));

            linearDodge_ = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Unload()
        {
            data_ = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void Decompress()
        {
            int decompressedSize = Width * Height * (ColorFormat == SgaColorFormat.Argb8888 ? 4 : 2);

            byte[]? decompressed = null;
            byte[] readData = data_!;

            if (CompressMode == SgaCompressMode.Zlib)
            {
                decompressed = Zlib.Decompress(data_!, decompressedSize);
                readData = decompressed;
            }

            if (ColorFormat == SgaColorFormat.Argb8888)
            {
                data_ = decompressed == null ? readData : decompressed;
                return;
            }

            int bit32Size = decompressedSize * 2;
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
            data_ = bit32Data;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
            => $"{FrameIndex}" + (IsLink ? $"링크({TargetFrameIndex})" : "");
    }
}
