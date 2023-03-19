/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:25:36 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Security.Policy;
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
using SGToolsCommon;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{


    [ExpandableObject]
    public struct SGUISpriteInfo
    {
        public static readonly SGUISpriteInfo Empty = new();
        public SGUISpriteInfo()
        {
            Sga = null;
            Img = null;
            Sprite = null;
        }

        public SGUISpriteInfo(SgaSprite sprite)
        {
            Debug.Assert(sprite != null);

            Sga = sprite.Parent.Parent;
            Img = sprite.Parent;
            Sprite = sprite;
        }

        public SGUISpriteInfo(SgaPackage sga, SgaImage img, SgaSprite sprite)
        {
            Debug.Assert(sprite != null);

            Sga = sga;
            Img = img;
            Sprite = sprite;
        }


        public SgaPackage Sga { get; }
        public SgaImage Img { get; }
        public SgaSprite Sprite { get; }

        [ReadOnly(true), DisplayName("선형 닷지")]
        public bool LinearDodge
        {
            get
            {
                if (Sprite == null)
                    return false;

                return Sprite.IsLinearDodged;
            }
            set
            {
                if (Sprite == null)
                    return;

                Sprite.IsLinearDodged = value;
            }
        }

        [Browsable(false)]
        public Rect Rect
        {
            get
            {
                if (Sprite == null)
                    throw new Exception("스프라이트가 세팅되어있지 않습니다.");

                return Sprite.Rect;
            }
        }

        [Browsable(false)]
        public int Height
        {
            get
            {
                if (Sprite == null)
                    return 0;

                return Sprite.Height;
            }
        }

        [Browsable(false)]
        public int Width
        {
            get
            {
                if (Sprite == null)
                    return 0;

                return Sprite.Width;
            }
        }

        [Browsable(false)]
        public Size Size
        {
            get
            {
                if (Sprite == null)
                    return new Size(0, 0);

                return Sprite.Size;
            }
        }

        [Browsable(false)]
        public BitmapSource Source => Sprite != null ? Sprite.Source : null;

        [Browsable(false)]
        public int SpriteIndex => Sprite != null ? Sprite.FrameIndex : Constant.InvalidValue;


        [Browsable(false)]
        public bool IsNull => Sprite == null;
        [Browsable(false)]
        public string SgaName => Sga.FileNameWithoutExt;
        [Browsable(false)]
        public string ImgName => Img.Header.NameWithoutExt;

        public override string ToString()
        {
            if (IsNull)
                return "비어있음";

            return $"{Img}-{Sprite.FrameIndex}";
        }

        [Browsable(false)]
        public string Question

        {
            get
            {
                if (IsNull)
                    return "비어있음";

                return $"Sga: {Sga}\n" +
                       $"Img: {Img}\n" +
                       $"Sprite: {Sprite}";
            }
        }
    }


    public static class SGUISpriteInfoExt
    {
        public static string ToFullString(this SGUISpriteInfo[] arr)
        {
            StringBuilder sb = new StringBuilder(64);

            for (int i = 0; i < arr.Length; ++i)
            {
                if (i == arr.Length - 1)
                    sb.Append(arr[i].SpriteIndex);
                else
                {
                    sb.Append(arr[i].SpriteIndex);
                    sb.Append(' ');
                }
            }

            return sb.ToString();
        }

        public static void ParseInfo(SgaPackage sga, SgaImage img, in int[] sourceSprites, in SGUISpriteInfo[] targetSprites, bool linearDodge = false)
        {
            // 무조건 두개 길이 같아야함.
            Debug.Assert(sourceSprites.Length == targetSprites.Length);

            for (int i = 0; i < sourceSprites.Length; ++i)
            {
                if (sourceSprites[i] != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(sourceSprites[i]) as SgaSprite;
                    if (sprite == null) throw new Exception($"{sga.FileNameWithoutExt} -> {img.Header.NameWithoutExt} -> {sourceSprites[i]}가 SgaSprite 타입이 아닙니다.");
                    targetSprites[i] = new SGUISpriteInfo(sga, img, sprite);
                    targetSprites[i].LinearDodge = linearDodge;
                }
            }
        }

        public static bool TryGetSgaImgFileName(in SGUISpriteInfo[] sprites, out string sga, out string img)
        {
            sga = string.Empty;
            img = string.Empty;

            SGUISpriteInfo info = sprites.FirstOrDefault(s => !s.IsNull);

            if (!info.IsNull)
            {
                sga = info.SgaName;
                img = info.ImgName;
                return true;
            }

            return false;
        }

    }
}
