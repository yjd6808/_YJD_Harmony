/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:50:18 AM
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
    public abstract class SgaSpriteAbstract
    {
        public SgaSpriteAbstract(SgaSpriteType spriteType, SgaColorFormat colorFormat, SgaImage parent, int frameIndex)
        {
            SpriteType = spriteType;
            ColorFormat = colorFormat;
            Parent = parent;
            FrameIndex = frameIndex;
        }
        public SgaSpriteType SpriteType { get; }
        public SgaColorFormat ColorFormat { get; }
        public SgaImage Parent { get; }
        public int FrameIndex { get; }
        public bool IsLink => SpriteType == SgaSpriteType.LinkSprite;
        public abstract bool IsDummy { get; }
        public abstract bool Loaded { get; }
        public abstract int Width { get; }
        public abstract int Height { get; }
        public abstract int X { get; }
        public abstract int Y { get; }
        public abstract int FrameWidth { get; }
        public abstract int FrameHeight { get; }
        public abstract int TargetFrameIndex { get; }

        public abstract void Load();
        public abstract void Unload();

        SgaSprite ToSprite()
        {
            if (SpriteType != SgaSpriteType.Sprite)
                throw new Exception("스프라이트 타입이 아닙니다.");

            return (SgaSprite)this;
        }

        SgaLinkSprite ToLinkSprite()
        {
            if (SpriteType != SgaSpriteType.LinkSprite)
                throw new Exception("링크 스프라이트 타입이 아닙니다.");

            return (SgaLinkSprite)this;
        }
            
    }
}
