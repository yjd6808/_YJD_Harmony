/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:50:18 AM
 *
 */

using System;
using System.Drawing;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Size = System.Windows.Size;

namespace SGToolsCommon.Sga
{
    public abstract class SgaSpriteAbstract
    {
        public SgaSpriteType SpriteType { get; }
        public SgaColorFormat ColorFormat { get; }
        public SgaImage? Parent { get; }
        public int FrameIndex { get; }
        public bool IsLink => SpriteType == SgaSpriteType.LinkSprite;
        public ListBoxItem? Item { get; set; }

        public abstract SgaSpriteRect SpriteRect { get; }
        public abstract Rect Rect { get; }
        public abstract Size Size { get; }
        public abstract Size FrameSize { get; }
        public abstract bool IsDummy { get; }
        public abstract bool Loaded { get; }
        public abstract int Width { get; }
        public abstract int Height { get; }
        public abstract int X { get; }
        public abstract int Y { get; }
        public abstract int FrameWidth { get; }
        public abstract int FrameHeight { get; }
        public abstract int TargetFrameIndex { get; }
        public abstract BitmapSource Source { get; }
        public abstract Bitmap Bitmap { get; }

        public string InfoString
        {
            get
            {
                if (IsLink)
                    return $"[{FrameIndex}] {Width}x{Height} (링크-{TargetFrameIndex})";

                return $"[{FrameIndex}] {Width}x{Height}";
            }
        }

        public string DataDir => $"{Parent!.Parent.FileName}\\{Parent!.Header.Name}";
        public string DataPath
        {
            get
            {
                if (IsLink)
                    return $"{Parent!.Parent.FileName}\\{Parent!.Header.Name}\\{FrameIndex}-링크{TargetFrameIndex}";

                return $"{Parent!.Parent.FileName}\\{Parent!.Header.Name}\\{FrameIndex}";
            }
        }

        public string SizeString => $"{Width}x{Height}";

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSpriteAbstract(SgaSpriteType _spriteType, SgaColorFormat _colorFormat, SgaImage? _parent, int _frameIndex)
        {
            SpriteType = _spriteType;
            ColorFormat = _colorFormat;
            Parent = _parent;
            FrameIndex = _frameIndex;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public abstract void Load();

        //////////////////////////////////////////////////////////////////////////////////
        public abstract void Unload();

        //////////////////////////////////////////////////////////////////////////////////
        private SgaSprite ToSprite()
        {
            if (SpriteType != SgaSpriteType.Sprite)
                throw new Exception("스프라이트 타입이 아닙니다.");

            return (SgaSprite)this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private SgaLinkSprite ToLinkSprite()
        {
            if (SpriteType != SgaSpriteType.LinkSprite)
                throw new Exception("링크 스프라이트 타입이 아닙니다.");

            return (SgaLinkSprite)this;
        }
    }
}
