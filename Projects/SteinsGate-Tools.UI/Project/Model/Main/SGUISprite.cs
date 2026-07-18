/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.Xml.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.SpriteCategoryName, Constant.OtherCategoryOrder)]
    public class SGUISprite : SGUIElement, ISizeRestorable
    {
        public const int OrderSize = 1;
        public const int OrderSprite = 2;
        public const int OrderLinearDodge = 3;
        public const int OrderScale9 = 4;

        public static int Seq;

        private SGUISpriteInfo sprite_;
        private bool linearDodge_;
        private bool scale9_;
        //////////////////////////////////////////////////////////////////////////////////
        public SGUISprite()
        {
            sprite_ = new SGUISpriteInfo();
        }

        [ReadOnly(false)]
        [Category(Constant.SpriteCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => visualSize_;
            set
            {
                visualSize_ = value;
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
                if (sprite_.IsNull) return 1.0;
                return (double)visualSize_.Width / sprite_.Sprite!.Width;
            }
        }

        [Browsable(false)]
        public double ScaleY
        {
            get
            {
                if (sprite_.IsNull) return 1.0;
                return (double)visualSize_.Height / sprite_.Sprite!.Height;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
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
                if (linearDodge_ && !sprite_.IsNull)
                    sprite_.LinearDodge = linearDodge_;

                return sprite_.Source;
            }
        }

        [Category(Constant.SpriteCategoryName), DisplayName("스프라이트"), PropertyOrder(OrderSprite)]
        public SGUISpriteInfo Sprite
        {
            get => sprite_;
            set
            {
                sprite_ = value;

                if (!sprite_.IsNull)
                    visualSize_ = sprite_.Sprite!.Rect.Size;
                else
                    visualSize_ = Constant.DefaultVisualSize;

                sprite_.LinearDodge = linearDodge_;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.SpriteCategoryName), DisplayName("선형 닷지"), PropertyOrder(OrderLinearDodge)]
        public bool LinearDodge
        {
            get => linearDodge_;
            set
            {
                sprite_.LinearDodge = value;
                linearDodge_ = value;
                OnPropertyChanged(nameof(SpriteSource));
            }
        }

        [Category(Constant.SpriteCategoryName), DisplayName("Scale9"), PropertyOrder(OrderScale9)]
        public bool Scale9
        {
            get => scale9_;
            set => scale9_ = value;
        }

        public override SGUIElementType UIElementType => SGUIElementType.Sprite;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "Sprite";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            SGUISpriteInfoExt.TryGetSgaImgFileName(sprite_, out string sga, out string img);

            root.SetAttributeValue("sga", sga);
            root.SetAttributeValue("img", img);
            root.SetAttributeValue("sprite", sprite_.SpriteIndex);
            root.SetAttributeValue("width", visualSize_.Width);
            root.SetAttributeValue("height", visualSize_.Height);

            if (linearDodge_)
                root.SetAttributeValue("linear_dodge", true);
            if (scale9_)
                root.SetAttributeValue("scale9", true);

            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
			try
			{
				XAttribute linearDodgeAttr = _root.Attribute("linear_dodge");
				if (linearDodgeAttr != null)
					linearDodge_ = (bool)linearDodgeAttr;
				XAttribute scale9Attr = _root.Attribute("scale9");
				if (scale9Attr != null)
					scale9_ = (bool)scale9Attr;

				string sgaName = (string)_root.Attribute("sga") ?? string.Empty;

				if (sgaName == string.Empty)
					return;

				string imgName = (string)_root.Attribute("img")!;

				SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
				SgaPackage sga = img.Parent;
				int spriteIndex = (int)_root.Attribute("sprite")!;

				if (spriteIndex == Constant.InvalidValue)
					return;

				SgaSprite? sprite = img.GetSprite(spriteIndex) as SgaSprite;
				if (sprite == null)
					throw new Exception($"{sgaName} -> {imgName} -> {spriteIndex}가 SgaSprite 타입이 아닙니다.");
				sprite_ = new SGUISpriteInfo(sga, img, sprite);
				sprite_.LinearDodge = linearDodge_;
			}
			finally
			{
				base.ParseXElement(_root);
			}
			
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUISprite sprite = new SGUISprite();
            sprite.CopyFrom(this);
            sprite.visualSize_ = visualSize_;
            sprite.sprite_ = sprite_;
            sprite.linearDodge_ = linearDodge_;
            sprite.scale9_ = scale9_;
            return sprite;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"스프라이트_{Seq++}";

        //////////////////////////////////////////////////////////////////////////////////
        public void RestoreSize()
        {
            if (sprite_.IsNull)
            {
                VisualSize = Constant.DefaultVisualSize;
                return;
            }

            VisualSize = sprite_.Rect.Size;
        }
    }
}
