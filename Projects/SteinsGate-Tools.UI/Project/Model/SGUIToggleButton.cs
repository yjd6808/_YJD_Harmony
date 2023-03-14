/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 6:31:58 PM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.FileSystem;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
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

        public SGUIToggleButton()
        {
            _sprites = new SGUISpriteInfo[2][];
            _sprites[0] = new SGUISpriteInfo[StateCount];
            _sprites[1] = new SGUISpriteInfo[StateCount];
        }


        [Browsable(false)] // 캔버스상에 보일 상태
        public SGUISpriteInfo VisualSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = 0; i < StateCount; ++i)
                        if (i == _state)
                            return _sprites[_toggleState][i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = 0; i < StateCount; ++i)
                    if (i == _state && !_sprites[_toggleState][i].IsNull)
                        return _sprites[_toggleState][i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return _sprites[_toggleState].FirstOrDefault(sprite => !sprite.IsNull);
            }
        }

        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(SGUIElement.OrderVisualSize)]
        public override Size VisualSize
        {
            get
            {
                for (int i = 0; i < StateCount; ++i)
                    if (!_sprites[_toggleState][i].IsNull)
                        return _sprites[_toggleState][i].Rect.Size;

                return Constant.DefaultVisualSize;
            }
        }

        [Browsable(false)]
        public BitmapSource VisualSpriteSource => VisualSprite.Source;

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
            get => _sprites[0][StateNormal];
            set
            {
                _sprites[0][StateNormal] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }


        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:마우스위"), PropertyOrder(OrderOver)]
        public SGUISpriteInfo Over
        {
            get => _sprites[0][StateOver];
            set
            {
                _sprites[0][StateOver] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:누름"), PropertyOrder(OrderPressed)]
        public SGUISpriteInfo Pressed
        {
            get => _sprites[0][StatePressed];
            set
            {
                _sprites[0][StatePressed] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("기본:비활성화"), PropertyOrder(OrderDisabled)]
        public SGUISpriteInfo Disabled
        {
            get => _sprites[0][StateDisabled];
            set
            {
                _sprites[0][StateDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }



        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:일반"), PropertyOrder(OrderToggledNormal)]
        public SGUISpriteInfo ToggleNormal
        {
            get => _sprites[1][StateNormal];
            set
            {
                _sprites[1][StateNormal] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }


        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:마우스위"), PropertyOrder(OrderToggledOver)]
        public SGUISpriteInfo ToggleOver
        {
            get => _sprites[1][StateOver];
            set
            {
                _sprites[1][StateOver] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:누름"), PropertyOrder(OrderToggledPressed)]
        public SGUISpriteInfo TogglePressed
        {
            get => _sprites[1][StatePressed];
            set
            {
                _sprites[1][StatePressed] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("토글:비활성화"), PropertyOrder(OrderToggledDisabled)]
        public SGUISpriteInfo ToggleDisabled
        {
            get => _sprites[1][StateDisabled];
            set
            {
                _sprites[1][StateDisabled] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ToggleButtonCategoryName), DisplayName("상태"), PropertyOrder(OrderToggleState)]
        public bool ToggleState
        {
            get => _toggleState == 1;
            set
            {
                _toggleState = value ? 1 : 0;
                NotifySpriteChanged();
            }
        }


        [Browsable(false)]
        public override int State
        {
            get => _state;
            set
            {
                _state = value;
                NotifySpriteChanged();
            }
        }


        public override SGUIElementType UIElementType => SGUIElementType.ToggleButton;
        [Browsable(false)] public override bool Manipulatable => false;


        public override object Clone()
        {
            SGUIToggleButton button = new SGUIToggleButton();
            button.CopyFrom(this);

            for (int k = 0; k < 2; ++k)
                for (int i = 0; i < StateCount; ++i)
                    button._sprites[k][i] = _sprites[k][i];

            return button;
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
            root[JsonSpriteKey] = $"{_sprites[0][0].SpriteIndex} {_sprites[0][1].SpriteIndex} {_sprites[0][2].SpriteIndex} {_sprites[0][3].SpriteIndex}";
            root[JsonToggleSpriteKey] = $"{_sprites[1][0].SpriteIndex} {_sprites[1][1].SpriteIndex} {_sprites[1][2].SpriteIndex} {_sprites[1][3].SpriteIndex}";
            return root;
        }

        private void GetSgaImgFileName(out string sga, out string img)
        {
            sga = string.Empty;
            img = string.Empty;
            int i;

            for (i = 0; i < StateCount; ++i)
                if (!_sprites[0][i].IsNull)
                    break;

            if (i != StateCount)
            {
                sga = _sprites[0][i].Sga.FileNameWithoutExt;
                img = _sprites[0][i].Img.Header.NameWithoutExt;
                return;
            }

            for (i = 0; i < StateCount; ++i)
                if (!_sprites[1][i].IsNull)
                    break;

            if (i == StateCount)
                return;

            sga = _sprites[1][i].Sga.FileNameWithoutExt;
            img = _sprites[1][i].Img.Header.NameWithoutExt;
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sgaName = (string)root[JsonSgaKey];

            if (sgaName == string.Empty)
                return;

            string imgName = (string)root[JsonImgKey];

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;

            int[] sprites = new int[StateCount];
            int[] toggledSprites = new int[StateCount];

            StringEx.ParseIntNumberN((string)root[JsonSpriteKey], sprites);
            StringEx.ParseIntNumberN((string)root[JsonToggleSpriteKey], toggledSprites);

            for (int i = 0; i < StateCount; ++i)
            {
                if (sprites[i] != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(sprites[i]) as SgaSprite;
                    if (sprite == null)
                        throw new Exception($"{sgaName} -> {imgName} -> {sprites[i]}가 SgaSprite 타입이 아닙니다.");
                    _sprites[0][i] = new SGUISpriteInfo(sga, img, sprite);
                }
            }

            for (int i = 0; i < StateCount; ++i)
            {
                if (toggledSprites[i] != Constant.InvalidValue)
                {
                    SgaSprite sprite = img.GetSprite(toggledSprites[i]) as SgaSprite;
                    if (sprite == null)
                        throw new Exception($"{sgaName} -> {imgName} -> {toggledSprites[i]}가 SgaSprite 타입이 아닙니다.");
                    _sprites[1][i] = new SGUISpriteInfo(sga, img, sprite);
                }
            }
        }


        public override bool OnMouseMove(Point p)
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

        public override bool OnMouseDown(Point p)
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

        public override bool OnMouseUp(Point p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(p);
            State = StateNormal;

            if (!contained)
                return true;

            ToggleState = !ToggleState;

            return false;
        }

        public override void CreateInit() => VisualName = $"토글버튼_{Seq++}";
        public static int Seq = 0;
        private SGUISpriteInfo[][] _sprites;
        private int _toggleState;
    }
}
