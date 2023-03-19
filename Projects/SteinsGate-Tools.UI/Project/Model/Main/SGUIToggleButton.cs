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
using MoreLinq;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.FileSystem;
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
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(OrderVisualSize)]
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
                _sprites[0][StateNormal].LinearDodge = _linearDodge;
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
                _sprites[0][StateOver].LinearDodge = _linearDodge;
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
                _sprites[0][StatePressed].LinearDodge = _linearDodge;
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
                _sprites[0][StateDisabled].LinearDodge = _linearDodge;
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
                _sprites[1][StateNormal].LinearDodge = _linearDodge;
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
                _sprites[1][StateOver].LinearDodge = _linearDodge;
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
                _sprites[1][StatePressed].LinearDodge = _linearDodge;
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
                _sprites[1][StateDisabled].LinearDodge = _linearDodge;
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

        
        [Category(Constant.ToggleButtonCategoryName), DisplayName("선형 닷지"), PropertyOrder(OrderLinearDodge)]
        public bool LinearDodge
        {
            get => _linearDodge;
            set
            {
                _sprites[0].ForEach(info => info.LinearDodge = value);
                _sprites[1].ForEach(info => info.LinearDodge = value);
                _linearDodge = value;
                OnPropertyChanged(nameof(VisualSpriteSource));
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

            if (!SGUISpriteInfoExt.TryGetSgaImgFileName(in _sprites[0], out sga, out img))
                SGUISpriteInfoExt.TryGetSgaImgFileName(in _sprites[1], out sga, out img);

            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = _sprites[0].ToFullString();
            root[JsonToggleSpriteKey] = _sprites[1].ToFullString();
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

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;

            int[] sprites = new int[StateCount];
            int[] toggledSprites = new int[StateCount];

            StringEx.ParseIntNumberN((string)root[JsonSpriteKey], sprites);
            StringEx.ParseIntNumberN((string)root[JsonToggleSpriteKey], toggledSprites);

            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in _sprites[0], _linearDodge);
            SGUISpriteInfoExt.ParseInfo(sga, img, in toggledSprites, in _sprites[1], _linearDodge);
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
        private bool _linearDodge;
        private int _toggleState;
    }
}
