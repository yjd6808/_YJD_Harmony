/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:25:36 AM
 *
 */

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Media.Imaging;
using SGToolsCommon;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [ExpandableObject]
    public struct SGUISpriteInfo
    {
        public static readonly SGUISpriteInfo Empty = new();

        //////////////////////////////////////////////////////////////////////////////////
        public SGUISpriteInfo()
        {
            Sga = null;
            Img = null;
            Sprite = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUISpriteInfo(SgaSprite _sprite)
        {
            Debug.Assert(_sprite != null);

            Sga = _sprite.Parent.Parent;
            Img = _sprite.Parent;
            Sprite = _sprite;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUISpriteInfo(SgaPackage _sga, SgaImage _img, SgaSprite _sprite)
        {
            Debug.Assert(_sprite != null);

            Sga = _sga;
            Img = _img;
            Sprite = _sprite;
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
        public string SgaName => Sga.FileName;
        [Browsable(false)]
        public string ImgName => Img.Header.Name;

        //////////////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this SGUISpriteInfo[] _arr)
        {
            StringBuilder sb = new StringBuilder(64);

            for (int i = 0; i < _arr.Length; ++i)
            {
                if (i == _arr.Length - 1)
                    sb.Append(_arr[i].SpriteIndex);
                else
                {
                    sb.Append(_arr[i].SpriteIndex);
                    sb.Append(' ');
                }
            }

            return sb.ToString();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseInfo(SgaPackage _sga, SgaImage _img, in int[] _sourceSprites, in SGUISpriteInfo[] _targetSprites, bool _linearDodge = false)
        {
            // 무조건 두개 길이 같아야함.
            Debug.Assert(_sourceSprites.Length == _targetSprites.Length);

            for (int i = 0; i < _sourceSprites.Length; ++i)
            {
                if (_sourceSprites[i] != Constant.InvalidValue)
                {
                    SgaSprite sprite = _img.GetSprite(_sourceSprites[i]) as SgaSprite;
                    if (sprite == null) throw new Exception($"{_sga.FileNameWithoutExt} -> {_img.Header.NameWithoutExt} -> {_sourceSprites[i]}가 SgaSprite 타입이 아닙니다.");
                    _targetSprites[i] = new SGUISpriteInfo(_sga, _img, sprite);
                    _targetSprites[i].LinearDodge = _linearDodge;
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool TryGetSgaImgFileName(in SGUISpriteInfo[] _sprites, out string _sga, out string _img)
        {
            _sga = string.Empty;
            _img = string.Empty;

            SGUISpriteInfo info = _sprites.FirstOrDefault(s => !s.IsNull);

            if (!info.IsNull)
            {
                _sga = info.SgaName;
                _img = info.ImgName;
                return true;
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool TryGetSgaImgFileName(in SGUISpriteInfo _sprite, out string _sga, out string _img)
        {
            _sga = string.Empty;
            _img = string.Empty;

            if (!_sprite.IsNull)
            {
                _sga = _sprite.SgaName;
                _img = _sprite.ImgName;
                return true;
            }

            return false;
        }
    }
}
