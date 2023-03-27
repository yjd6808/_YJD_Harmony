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
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.SpriteCategoryName, Constant.OtherCategoryOrder)]
    public class SGUISprite : SGUIElement, ISizeRestorable
    {
        public const int OrderSize = 1;
        public const int OrderSprite = 2;
        public const int OrderLinearDodge = 3;

        public SGUISprite()
        {
            _sprite = new SGUISpriteInfo();
            _visualSize = Constant.DefaultVisualSize;
        }


        [ReadOnly(false)]
        [Category(Constant.SpriteCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => _visualSize;
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
                return (double)_visualSize.Width / _sprite.Sprite.Width;
            }
        }

        [Browsable(false)]
        public double ScaleY
        {
            get
            {
                if (_sprite.IsNull) return 1.0;
                return (double)_visualSize.Height / _sprite.Sprite.Height;
            }
        }


        private void NotifySpriteChanged()
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
            OnPropertyChanged(nameof(SpriteSource));
        }

        [Browsable(false)]
        public BitmapSource SpriteSource
        {
            get
            {
                // 팩 언로딩되었다가 다시 로딩된 경우 텍스쳐의 선형 닷지가 초기화되어있기때문에, 소스 가져올때 재적용해줘야함
                if (_linearDodge && !_sprite.IsNull)
                    _sprite.LinearDodge = _linearDodge;

                return _sprite.Source;
            }
        }


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

                _sprite.LinearDodge = _linearDodge;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.SpriteCategoryName), DisplayName("선형 닷지"), PropertyOrder(OrderLinearDodge)]
        public bool LinearDodge
        {
            get => _linearDodge;
            set
            {
                _sprite.LinearDodge = value;
                _linearDodge = value;
                OnPropertyChanged(nameof(SpriteSource));
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
            SGUISpriteInfoExt.TryGetSgaImgFileName(_sprite, out string sga, out string img);

            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = _sprite.SpriteIndex;
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            root[JsonLinearDodgeKey] = _linearDodge;
            return root;
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            root.TryGetValueDefault(JsonLinearDodgeKey, out _linearDodge, false);

            string sgaName = (string)root[JsonSgaKey];

            if (sgaName == string.Empty)
                return;

            string imgName = (string)root[JsonImgKey];
            string sizeString = (string)root[JsonVisualSizeKey];

            _visualSize = SizeEx.ParseFullString(sizeString);

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int spriteIndex = (int)root[JsonSpriteKey];

            if (spriteIndex == Constant.InvalidValue)
                return;

            SgaSprite sprite = img.GetSprite(spriteIndex) as SgaSprite;
            if (sprite == null)
                throw new Exception($"{sgaName} -> {imgName} -> {spriteIndex}가 SgaSprite 타입이 아닙니다.");
            _sprite = new SGUISpriteInfo(sga, img, sprite);
            _sprite.LinearDodge = _linearDodge;
        }

        public override void CreateInit() => VisualName = $"스프라이트_{Seq++}";
        public static int Seq;
        private SGUISpriteInfo _sprite;
        private bool _linearDodge;
        private IntSize _visualSize;

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
