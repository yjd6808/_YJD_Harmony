/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.Reflection;
using System.Security.Cryptography;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
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

        public SGUICheckBox()
        {
            _sprites = new SGUISpriteInfo[4];
        }


        [Browsable(false)]
        public BitmapSource VisualBackgroundSource => VisualBackgroundSprite.Source;

        [Browsable(false)]
        public BitmapSource VisualCrossSource => VisualCrossSprite.Source;

        [Browsable(false)]
        public SGUISpriteInfo VisualCrossSprite =>
            _state != StateDisabled ? _sprites[IndexCross] : _sprites[IndexCrossDisabled];

        [Browsable(false)]
        public SGUISpriteInfo VisualBackgroundSprite =>
            _state != StateDisabled ? _sprites[IndexBackground] : _sprites[IndexBackgroundDisabled];

        public bool HasBackground => !_sprites[IndexBackground].IsNull || !_sprites[IndexBackgroundDisabled].IsNull;


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

                return Constant.DefaultVisualSize;
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("체크"), PropertyOrder(OrderChecked)]
        public bool Checked
        {
            get => _checked;
            set
            {
                _checked = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(CheckedOpacity));
            }
        }

        [Browsable(false)] public double CheckedOpacity => _checked ? 1.0 : 0.0;

        [Category(Constant.CheckBoxCategoryName), DisplayName("배경"), PropertyOrder(OrderBackground)]
        public SGUISpriteInfo Background
        {
            get => _sprites[IndexBackground];
            set
            {
                _sprites[IndexBackground] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyBackground);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("배경(비활성)"), PropertyOrder(OrderBackgroundDisabled)]
        public SGUISpriteInfo BackgroundDisabled
        {
            get => _sprites[IndexBackgroundDisabled];
            set
            {
                _sprites[IndexBackgroundDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyBackground);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("크로스"), PropertyOrder(OrderCross)]
        public SGUISpriteInfo Cross
        {
            get => _sprites[IndexCross];
            set
            {
                _sprites[IndexCross] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyCross);
            }
        }

        [Category(Constant.CheckBoxCategoryName), DisplayName("크로스(비활성)"), PropertyOrder(OrderCrossDisabled)]
        public SGUISpriteInfo CrossDisabled
        {
            get => _sprites[IndexCrossDisabled];
            set
            {
                _sprites[IndexCrossDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged(NotifyCross);
            }
        }

        private void NotifySpriteChanged(int notifyType)
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));

            switch (notifyType)
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

        public override object Clone()
        {
            SGUICheckBox checkBox = new SGUICheckBox();
            checkBox.CopyFrom(this);
            checkBox._checked = _checked;
            Array.Copy(_sprites, checkBox._sprites, _sprites.Length);
            return checkBox;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();

            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            GetSgaImgFileName(true, out string bgSga, out string bgImg);
            GetSgaImgFileName(false, out string crossSga, out string crossImg);

            if (_checked)
                root[JsonCheck] = true;

            root[JsonBackgroundSga] = bgSga;
            root[JsonBackgroundImg] = bgImg;
            root[JsonCrossSga] = crossSga;
            root[JsonCrossImg] = crossImg;
            root[JsonSpriteKey] = $"{_sprites[0].SpriteIndex} {_sprites[1].SpriteIndex} {_sprites[2].SpriteIndex} {_sprites[3].SpriteIndex}";
            return root;
        }

        private void GetSgaImgFileName(bool background, out string sga, out string img)
        {
            sga = string.Empty;
            img = string.Empty;

            int normal = background ? IndexBackground : IndexCross;
            int disabled = background ? IndexBackgroundDisabled : IndexCrossDisabled;

            if (!_sprites[normal].IsNull)
            {
                sga = _sprites[normal].Sga.FileName;
                img = _sprites[normal].Img.Header.Name;
                return;
            }

            if (!_sprites[disabled].IsNull)
            {
                sga = _sprites[disabled].Sga.FileName;
                img = _sprites[disabled].Img.Header.Name;
            }
        }

        [Browsable(false)]
        public override int State
        {
            get => _state;
            set
            {
                _state = value;
                NotifySpriteChanged(NotifyAll);
            }
        }
        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            root.TryGetValueDefault(JsonCheck, out _checked, false);

            string bg_sga = (string)root[JsonBackgroundSga];
            string bg_img = (string)root[JsonBackgroundImg];

            string cross_sga = (string)root[JsonCrossSga];
            string cross_img = (string)root[JsonCrossImg];

            int[] sprites = new int[4];
            StringEx.ParseIntNumberN((string)root[JsonSpriteKey], sprites);

            if (bg_sga != string.Empty)
            {
                SgaImage img = ViewModel.PackManager.GetImg(bg_sga, bg_img);
                SgaPackage sga = img.Parent;

                int background_frameindex = sprites[IndexBackground];
                int background_disabled_frameindex = sprites[IndexBackgroundDisabled];

                if (background_frameindex != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(background_frameindex) as SgaSprite;
                    _sprites[IndexBackground] = new SGUISpriteInfo(sga, img, sprite);
                }

                if (background_disabled_frameindex != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(background_disabled_frameindex) as SgaSprite;
                    _sprites[IndexBackgroundDisabled] = new SGUISpriteInfo(sga, img, sprite);
                }
            }

            if (cross_sga != string.Empty)
            {
                SgaImage img = ViewModel.PackManager.GetImg(cross_sga, cross_img);
                SgaPackage sga = img.Parent;

                int crossFrameindex = sprites[IndexCross];
                int crossDisabledFrameindex = sprites[IndexCrossDisabled];

                if (crossFrameindex != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(crossFrameindex) as SgaSprite;
                    _sprites[IndexCross] = new SGUISpriteInfo(sga, img, sprite);
                }

                if (crossDisabledFrameindex != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(crossDisabledFrameindex) as SgaSprite;
                    _sprites[IndexCrossDisabled] = new SGUISpriteInfo(sga, img, sprite);
                }
            }
        }

        public override bool OnMouseMove(IntPoint p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(p);

            if (!contained)
            {
                State = StateNormal;
                return true;
            }

            State = StateOver;
            return false;
        }

        public override bool OnMouseDown(IntPoint p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(p);
            if (!contained)
                return true;

            State = StatePressed;
            return false;
        }

        public override bool OnMouseUp(IntPoint p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(p);
            State = StateNormal;

            if (!contained)
                return true;

            Checked = !Checked;

            return false;
        }



        public override void CreateInit() => VisualName = $"체크박스_{Seq++}";
        public static int Seq;
        private bool _checked = true;
        private SGUISpriteInfo[] _sprites;
    }

}
