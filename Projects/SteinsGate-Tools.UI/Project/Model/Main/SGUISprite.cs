/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.SpriteCategoryName, Constant.OtherCategoryOrder)]
    public class SGUISprite : SGUIElement, ISizeRestorable
    {
        public const int OrderSize = 1;
        public const int OrderSprite = 1;

        public SGUISprite()
        {
            _sprite = new SGUISpriteInfo();
            _visualSize = Constant.DefaultVisualSize;
        }


        [ReadOnly(false)]
        [Category(Constant.SpriteCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override Size VisualSize
        {
            get => new((int)_visualSize.Width, (int)_visualSize.Height);
            set
            {
                _visualSize = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(ScaleX));
                OnPropertyChanged(nameof(ScaleY));
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Browsable(false)]
        public double ScaleX
        {
            get
            {
                if (_sprite.IsNull) return 1.0;
                return _visualSize.Width / _sprite.Sprite.Width;
            }
        }

        [Browsable(false)]
        public double ScaleY
        {
            get
            {
                if (_sprite.IsNull) return 1.0;
                return _visualSize.Height / _sprite.Sprite.Height;
            }
        }


        private void NotifySpriteChanged()
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
            OnPropertyChanged(nameof(SpriteSource));
        }

        [Browsable(false)]
        public BitmapSource SpriteSource => Sprite.Source;


        [Category(Constant.SpriteCategoryName), DisplayName("스프라이트"), PropertyOrder(OrderSprite)]
        public SGUISpriteInfo Sprite
        {
            get => _sprite;
            set
            {
                _sprite = value;

                if (!_sprite.IsNull)
                    _visualSize = _sprite.Sprite.Rect.Size;
                else
                    _visualSize = Constant.DefaultVisualSize;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.Sprite;
        [Browsable(false)] public override bool Manipulatable => true;

        public override object Clone()
        {
            SGUISprite sprite = new SGUISprite();
            sprite.CopyFrom(this);
            sprite._visualSize = _visualSize;
            sprite._sprite = _sprite;
            return sprite;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            string sga;
            string img;
            GetSgaImgFileName(out sga, out img);

            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = _sprite.SpriteIndex.ToString();
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            return root;
        }

        private void GetSgaImgFileName(out string sga, out string img)
        {
            sga = string.Empty;
            img = string.Empty;

            if (!_sprite.IsNull)
            {
                sga = _sprite.Sga.FileNameWithoutExt;
                img = _sprite.Img.Header.NameWithoutExt;
            }
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sgaName = (string)root[JsonSgaKey];

            if (sgaName == string.Empty)
                return;

            string imgName = (string)root[JsonImgKey];
            string sizeString = (string)root[JsonVisualSizeKey];

            _visualSize = SizeEx.ParseFullString(sizeString);

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int spriteIndex = int.Parse((string)root[JsonSpriteKey]);

            if (spriteIndex == Constant.InvalidValue)
                return;

            SgaSprite sprite = img.GetSprite(spriteIndex) as SgaSprite;
            if (sprite == null)
                throw new Exception($"{sgaName} -> {imgName} -> {spriteIndex}가 SgaSprite 타입이 아닙니다.");
            _sprite = new SGUISpriteInfo(sga, img, sprite);
        }

        public override void CreateInit() => VisualName = $"스프라이트_{Seq++}";
        public static int Seq;
        private SGUISpriteInfo _sprite;
        private Size _visualSize;

        public void RestoreSize()
        {
            if (_sprite.IsNull)
            {
                VisualSize = Constant.DefaultVisualSize;
                return;
            }

            VisualSize = _sprite.Rect.Size;
        }
    }
}
