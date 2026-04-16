/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:23:45 AM
 *
 */

using System.ComponentModel;
using System.Linq;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.FileSystem;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.ButtonCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIButton : SGUIElement
    {
        public const int OrderNormal = 1;
        public const int OrderOver = 2;
        public const int OrderPressed = 3;
        public const int OrderDisabled = 4;
        public const int OrderLinearDodge = 5;

        public static int Seq = 0;

        private bool linearDodge_;
        private SGUISpriteInfo[] sprites_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIButton() => sprites_ = new SGUISpriteInfo[StateCount];

        [Browsable(false)] // 캔버스상에 보일 상태
        public SGUISpriteInfo VisualSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = 0; i < sprites_.Length; ++i)
                        if (i == state_)
                            return sprites_[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = 0; i < sprites_.Length; ++i)
                    if (i == state_ && !sprites_[i].IsNull)
                        return sprites_[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return sprites_.FirstOrDefault(sprite => !sprite.IsNull);
            }
        }

        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(OrderVisualSize)]
        public override IntSize VisualSize
        {
            get
            {
                for (int i = 0; i < StateCount; ++i)
                    if (!sprites_[i].IsNull)
                        return sprites_[i].Rect.Size;

                return Constant.DefaultVisualSize;
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

        [Category(Constant.ButtonCategoryName), DisplayName("일반"), PropertyOrder(OrderNormal)]
        public SGUISpriteInfo Normal
        {
            get => sprites_[StateNormal];
            set
            {
                sprites_[StateNormal] = value;
                sprites_[StateNormal].LinearDodge = linearDodge_;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("마우스위"), PropertyOrder(OrderOver)]
        public SGUISpriteInfo Over
        {
            get => sprites_[StateOver];
            set
            {
                sprites_[StateOver] = value;
                sprites_[StateOver].LinearDodge = linearDodge_;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("누름"), PropertyOrder(OrderPressed)]
        public SGUISpriteInfo Pressed
        {
            get => sprites_[StatePressed];
            set
            {
                sprites_[StatePressed] = value;
                sprites_[StatePressed].LinearDodge = linearDodge_;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("비활성화"), PropertyOrder(OrderDisabled)]
        public SGUISpriteInfo Disabled
        {
            get => sprites_[StateDisabled];
            set
            {
                sprites_[StateDisabled] = value;
                sprites_[StateDisabled].LinearDodge = linearDodge_;

                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("선형 닷지"), PropertyOrder(OrderLinearDodge)]
        public bool LinearDodge
        {
            get => linearDodge_;
            set
            {
                sprites_.ForEach(info => info.LinearDodge = value);
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

        public override SGUIElementType UIElementType => SGUIElementType.Button;
        [Browsable(false)] public override bool Manipulatable => false;

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIButton button = new SGUIButton();
            button.CopyFrom(this);

            for (int i = 0; i < StateCount; ++i)
                button.sprites_[i] = sprites_[i];

            return button;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            SGUISpriteInfoExt.TryGetSgaImgFileName(in sprites_, out string sga, out string img);
            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = sprites_.ToFullString();
            root[JsonLinearDodgeKey] = linearDodge_;
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseJObject(JObject _root)
        {
            base.ParseJObject(_root);

            _root.TryGetValueDefault(JsonLinearDodgeKey, out linearDodge_, false);
            _root.TryGetValueDefault(JsonSgaKey, out string? sgaName, string.Empty);

            if (sgaName == null || sgaName.Length == 0)
                return;

            string imgName = (string)_root[JsonImgKey]!;

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;

            int[] sprites = new int[StateCount];
            StringEx.ParseIntNumberN((string)_root[JsonSpriteKey]!, sprites);
            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in sprites_, linearDodge_);
        }

        // 기본적으로 엘리먼트의 이벤트는 "전파"되도록한다.
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

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"버튼_{Seq++}";
    }
}
