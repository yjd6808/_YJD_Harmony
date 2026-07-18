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
    [CategoryOrder(Constant.CheckBoxCategoryName, Constant.OtherCategoryOrder)]
    public class SGUICheckBox : SGUIElement
    {
        public const int IndexBackground = 0;
        public const int IndexBackgroundDisabled = 1;
        public const int IndexCross = 2;
        public const int IndexCrossDisabled = 3;

        public const int OrderChecked = 1;
        public const int OrderBackground = 2;
        public const int OrderBackgroundDisabled = 3;
        public const int OrderCross = 4;
        public const int OrderCrossDisabled = 5;

        public const int NotifyBackground = 0;
        public const int NotifyCross = 1;
        public const int NotifyAll = 2;

        public static int Seq;

        private bool checked_ = true;
        private SGUISpriteInfo[] sprites_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUICheckBox()
        {
            sprites_ = new SGUISpriteInfo[4];
        }

        [Browsable(false)]
        public BitmapSource VisualBackgroundSource => VisualBackgroundSprite.Source;

        [Browsable(false)]
        public BitmapSource VisualCrossSource => VisualCrossSprite.Source;

        [Browsable(false)]
        public SGUISpriteInfo VisualCrossSprite =>
            state_ != StateDisabled ? sprites_[IndexCross] : sprites_[IndexCrossDisabled];

        [Browsable(false)]
        public SGUISpriteInfo VisualBackgroundSprite =>
            state_ != StateDisabled ? sprites_[IndexBackground] : sprites_[IndexBackgroundDisabled];

        public bool HasBackground => !sprites_[IndexBackground].IsNull || !sprites_[IndexBackgroundDisabled].IsNull;

        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(OrderVisualSize)]
        public override IntSize VisualSize
        {
            get
            {
                bool visualBackgroundIsNull = VisualBackgroundSprite.IsNull;
                bool visualCrossIsNull = VisualCrossSprite.IsNull;

                if (!visualBackgroundIsNull && !visualCrossIsNull)
                {
                    Size size = new();
                    size.Width = Math.Max(VisualBackgroundSprite.Rect.Width, VisualCrossSprite.Rect.Width);
                    size.Height = Math.Max(VisualBackgroundSprite.Rect.Height, VisualCrossSprite.Rect.Height);
                    return size;
                }

                if (!visualBackgroundIsNull)
                    return VisualBackgroundSprite.Rect.Size;
                if (!visualCrossIsNull)
                    return VisualCrossSprite.Rect.Size;

                return visualSize_;
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("체크"), PropertyOrder(OrderChecked)]
        public bool Checked
        {
            get => checked_;
            set
            {
                checked_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(CheckedOpacity));
            }
        }

        [Browsable(false)] public double CheckedOpacity => checked_ ? 1.0 : 0.0;

        [Category(Constant.CheckBoxCategoryName), DisplayName("배경"), PropertyOrder(OrderBackground)]
        public SGUISpriteInfo Background
        {
            get => sprites_[IndexBackground];
            set
            {
                sprites_[IndexBackground] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyBackground);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("배경(비활성)"), PropertyOrder(OrderBackgroundDisabled)]
        public SGUISpriteInfo BackgroundDisabled
        {
            get => sprites_[IndexBackgroundDisabled];
            set
            {
                sprites_[IndexBackgroundDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyBackground);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("크로스"), PropertyOrder(OrderCross)]
        public SGUISpriteInfo Cross
        {
            get => sprites_[IndexCross];
            set
            {
                sprites_[IndexCross] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyCross);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("크로스(비활성)"), PropertyOrder(OrderCrossDisabled)]
        public SGUISpriteInfo CrossDisabled
        {
            get => sprites_[IndexCrossDisabled];
            set
            {
                sprites_[IndexCrossDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyCross);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void NotifySpriteChanged(int _notifyType)
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));

            switch (_notifyType)
            {
                case NotifyBackground:
                    OnPropertyChanged(nameof(VisualBackgroundSprite));
                    OnPropertyChanged(nameof(VisualBackgroundSource));
                    OnPropertyChanged(nameof(HasBackground));
                    break;
                case NotifyCross:
                    OnPropertyChanged(nameof(VisualCrossSprite));
                    OnPropertyChanged(nameof(VisualCrossSource));
                    break;
                case NotifyAll:
                    OnPropertyChanged(nameof(VisualBackgroundSprite));
                    OnPropertyChanged(nameof(VisualBackgroundSource));
                    OnPropertyChanged(nameof(HasBackground));
                    OnPropertyChanged(nameof(VisualCrossSprite));
                    OnPropertyChanged(nameof(VisualCrossSource));
                    break;
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.CheckBox;
        [Browsable(false)] public override bool Manipulatable => false;

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "CheckBox";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();

            GetSgaImgFileName(true, out string bgSga, out string bgImg);
            GetSgaImgFileName(false, out string crossSga, out string crossImg);

            if (checked_)
                root.SetAttributeValue("check", true);

            root.SetAttributeValue("bg_sga", bgSga);
            root.SetAttributeValue("bg_img", bgImg);
            root.SetAttributeValue("cross_sga", crossSga);
            root.SetAttributeValue("cross_img", crossImg);
            root.SetAttributeValue("sprite", $"{sprites_[0].SpriteIndex} {sprites_[1].SpriteIndex} {sprites_[2].SpriteIndex} {sprites_[3].SpriteIndex}");
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void GetSgaImgFileName(bool _background, out string _sga, out string _img)
        {
            _sga = string.Empty;
            _img = string.Empty;

            int normal = _background ? IndexBackground : IndexCross;
            int disabled = _background ? IndexBackgroundDisabled : IndexCrossDisabled;

            if (!sprites_[normal].IsNull)
            {
                _sga = sprites_[normal].Sga!.FileName;
                _img = sprites_[normal].Img!.Header.Name;
                return;
            }

            if (!sprites_[disabled].IsNull)
            {
                _sga = sprites_[disabled].Sga!.FileName;
                _img = sprites_[disabled].Img!.Header.Name;
            }
        }

        [Browsable(false)]
        public override int State
        {
            get => state_;
            set
            {
                state_ = value;
                NotifySpriteChanged(NotifyAll);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            XAttribute checkAttr = _root.Attribute("check");
            if (checkAttr != null)
                checked_ = (bool)checkAttr;

            string bgSga = (string)_root.Attribute("bg_sga") ?? string.Empty;
            string bgImg = (string)_root.Attribute("bg_img") ?? string.Empty;

            string crossSga = (string)_root.Attribute("cross_sga") ?? string.Empty;
            string crossImg = (string)_root.Attribute("cross_img") ?? string.Empty;

            int[] sprites = new int[4];
            string spriteStr = (string)_root.Attribute("sprite") ?? string.Empty;
            if (spriteStr.Length > 0)
                StringEx.ParseIntNumberN(spriteStr, sprites);

            if (bgSga != string.Empty)
            {
                SgaImage img = ViewModel.PackManager.GetImg(bgSga, bgImg);
                SgaPackage sga = img.Parent;

                int backgroundFrameindex = sprites[IndexBackground];
                int backgroundDisabledFrameindex = sprites[IndexBackgroundDisabled];

                if (backgroundFrameindex != Constant.InvalidValue)
                {
                    SgaSprite? sprite = img.GetSprite(backgroundFrameindex) as SgaSprite;
                    sprites_[IndexBackground] = new SGUISpriteInfo(sga, img, sprite!);
                }

                if (backgroundDisabledFrameindex != Constant.InvalidValue)
                {
                    SgaSprite? sprite = img.GetSprite(backgroundDisabledFrameindex) as SgaSprite;
                    sprites_[IndexBackgroundDisabled] = new SGUISpriteInfo(sga, img, sprite!);
                }
            }

            if (crossSga != string.Empty)
            {
                SgaImage img = ViewModel.PackManager.GetImg(crossSga, crossImg);
                SgaPackage sga = img.Parent;

                int crossFrameindex = sprites[IndexCross];
                int crossDisabledFrameindex = sprites[IndexCrossDisabled];

                if (crossFrameindex != Constant.InvalidValue)
                {
                    SgaSprite? sprite = img.GetSprite(crossFrameindex) as SgaSprite;
                    sprites_[IndexCross] = new SGUISpriteInfo(sga, img, sprite!);
                }

                if (crossDisabledFrameindex != Constant.InvalidValue)
                {
                    SgaSprite? sprite = img.GetSprite(crossDisabledFrameindex) as SgaSprite;
                    sprites_[IndexCrossDisabled] = new SGUISpriteInfo(sga, img, sprite!);
                }
            }

			base.ParseXElement(_root);
		}

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUICheckBox checkBox = new SGUICheckBox();
            checkBox.CopyFrom(this);
            checkBox.checked_ = checked_;
            Array.Copy(sprites_, checkBox.sprites_, sprites_.Length);
            return checkBox;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseMove(IntPoint _p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(_p);

            if (!contained)
            {
                State = StateNormal;
                return true;
            }

            State = StateOver;
            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseDown(IntPoint _p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(_p);
            if (!contained)
                return true;

            State = StatePressed;
            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseUp(IntPoint _p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(_p);
            State = StateNormal;

            if (!contained)
                return true;

            Checked = !Checked;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"체크박스_{Seq++}";
    }
}
