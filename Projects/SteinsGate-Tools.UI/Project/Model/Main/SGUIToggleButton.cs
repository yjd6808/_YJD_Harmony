/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 6:31:58 PM
 *
 */

using System.ComponentModel;
using System.Linq;
using System.Xml.Linq;
using System.Windows.Media.Imaging;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.ToggleButtonCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIToggleButton : SGUIElement
    {
        public const int OrderToggleState = 1;

        public const int OrderNormal = 2;
        public const int OrderOver = 3;
        public const int OrderPressed = 4;
        public const int OrderDisabled = 5;

        public const int OrderToggledNormal = 6;
        public const int OrderToggledOver = 7;
        public const int OrderToggledPressed = 8;
        public const int OrderToggledDisabled = 9;

        public const int OrderLinearDodge = 10;

        public static int Seq = 0;

        private SGUISpriteInfo[][] sprites_;
        private bool linearDodge_;
        private int toggleState_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIToggleButton()
        {
            sprites_ = new SGUISpriteInfo[2][];
            sprites_[0] = new SGUISpriteInfo[StateCount];
            sprites_[1] = new SGUISpriteInfo[StateCount];
        }

        [Browsable(false)] // 캔버스상에 보일 상태
        public SGUISpriteInfo VisualSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = 0; i < StateCount; ++i)
                        if (i == state_)
                            return sprites_[toggleState_][i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = 0; i < StateCount; ++i)
                    if (i == state_ && !sprites_[toggleState_][i].IsNull)
                        return sprites_[toggleState_][i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return sprites_[toggleState_].FirstOrDefault(sprite => !sprite.IsNull);
            }
        }

        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(OrderVisualSize)]
        public override IntSize VisualSize
        {
            get
            {
                for (int i = 0; i < StateCount; ++i)
                    if (!sprites_[toggleState_][i].IsNull)
                        return sprites_[toggleState_][i].Rect.Size;

                return Constant.DefaultVisualSize;
            }
        }

        [Browsable(false)]
        public BitmapSource VisualSpriteSource
        {
            get
            {
                SGUISpriteInfo visualSprite = VisualSprite;
                // 팩 언로딩되었다가 다시 로딩된 경우 텍스쳐의 선형 닷지가 초기화되어있기때문에, 소스 가져올때 재적용해줘야함
                if (linearDodge_ && !visualSprite.IsNull)
                    visualSprite.LinearDodge = linearDodge_;
                return visualSprite.Source;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void NotifySpriteChanged()
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
            OnPropertyChanged(nameof(VisualSprite));
            OnPropertyChanged(nameof(VisualSpriteSource));
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:일반"), PropertyOrder(OrderNormal)]
        public SGUISpriteInfo Normal
        {
            get => sprites_[0][StateNormal];
            set
            {
                sprites_[0][StateNormal] = value;
                sprites_[0][StateNormal].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:마우스위"), PropertyOrder(OrderOver)]
        public SGUISpriteInfo Over
        {
            get => sprites_[0][StateOver];
            set
            {
                sprites_[0][StateOver] = value;
                sprites_[0][StateOver].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:누름"), PropertyOrder(OrderPressed)]
        public SGUISpriteInfo Pressed
        {
            get => sprites_[0][StatePressed];
            set
            {
                sprites_[0][StatePressed] = value;
                sprites_[0][StatePressed].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:비활성화"), PropertyOrder(OrderDisabled)]
        public SGUISpriteInfo Disabled
        {
            get => sprites_[0][StateDisabled];
            set
            {
                sprites_[0][StateDisabled] = value;
                sprites_[0][StateDisabled].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:일반"), PropertyOrder(OrderToggledNormal)]
        public SGUISpriteInfo ToggleNormal
        {
            get => sprites_[1][StateNormal];
            set
            {
                sprites_[1][StateNormal] = value;
                sprites_[1][StateNormal].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:마우스위"), PropertyOrder(OrderToggledOver)]
        public SGUISpriteInfo ToggleOver
        {
            get => sprites_[1][StateOver];
            set
            {
                sprites_[1][StateOver] = value;
                sprites_[1][StateOver].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:누름"), PropertyOrder(OrderToggledPressed)]
        public SGUISpriteInfo TogglePressed
        {
            get => sprites_[1][StatePressed];
            set
            {
                sprites_[1][StatePressed] = value;
                sprites_[1][StatePressed].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:비활성화"), PropertyOrder(OrderToggledDisabled)]
        public SGUISpriteInfo ToggleDisabled
        {
            get => sprites_[1][StateDisabled];
            set
            {
                sprites_[1][StateDisabled] = value;
                sprites_[1][StateDisabled].LinearDodge = linearDodge_;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("상태"), PropertyOrder(OrderToggleState)]
        public bool ToggleState
        {
            get => toggleState_ == 1;
            set
            {
                toggleState_ = value ? 1 : 0;
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("선형 닷지"), PropertyOrder(OrderLinearDodge)]
        public bool LinearDodge
        {
            get => linearDodge_;
            set
            {
                sprites_[0].ForEach(info => info.LinearDodge = value);
                sprites_[1].ForEach(info => info.LinearDodge = value);
                linearDodge_ = value;
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }

        [Browsable(false)]
        public override int State
        {
            get => state_;
            set
            {
                state_ = value;
                NotifySpriteChanged();
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.ToggleButton;
        [Browsable(false)] public override bool Manipulatable => false;

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "ToggleButton";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            string sga;
            string img;

            if (!SGUISpriteInfoExt.TryGetSgaImgFileName(in sprites_[0], out sga, out img))
                SGUISpriteInfoExt.TryGetSgaImgFileName(in sprites_[1], out sga, out img);

            root.SetAttributeValue("sga", sga);
            root.SetAttributeValue("img", img);
            root.SetAttributeValue("sprite", sprites_[0].ToFullString());
            root.SetAttributeValue("sprite2", sprites_[1].ToFullString());
            if (linearDodge_)
                root.SetAttributeValue("linear_dodge", true);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            base.ParseXElement(_root);

            XAttribute linearDodgeAttr = _root.Attribute("linear_dodge");
            if (linearDodgeAttr != null)
                linearDodge_ = (bool)linearDodgeAttr;

            string sgaName = (string)_root.Attribute("sga") ?? string.Empty;

            if (sgaName == string.Empty)
                return;

            string imgName = (string)_root.Attribute("img")!;

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;

            int[] sprites = new int[StateCount];
            int[] toggledSprites = new int[StateCount];

            string spriteStr = (string)_root.Attribute("sprite") ?? string.Empty;
            if (spriteStr.Length > 0)
                StringEx.ParseIntNumberN(spriteStr, sprites);

            string toggledSpriteStr = (string)_root.Attribute("sprite2") ?? string.Empty;
            if (toggledSpriteStr.Length > 0)
                StringEx.ParseIntNumberN(toggledSpriteStr, toggledSprites);

            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in sprites_[0], linearDodge_);
            SGUISpriteInfoExt.ParseInfo(sga, img, in toggledSprites, in sprites_[1], linearDodge_);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIToggleButton button = new SGUIToggleButton();
            button.CopyFrom(this);

            for (int k = 0; k < 2; ++k)
                for (int i = 0; i < StateCount; ++i)
                    button.sprites_[k][i] = sprites_[k][i];

            return button;
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

            ToggleState = !ToggleState;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"토글버튼_{Seq++}";
    }
}
