/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:23:45 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.FileSystem;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{

    [CategoryOrder(Constant.ButtonCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIButton : SGUIElement
    {
        public const int OrderNormal = 1;
        public const int OrderPressed = 2;
        public const int OrderOver = 3;
        public const int OrderDisabled = 4;


        public SGUIButton()
        {
            _sprites = new SGUISpriteInfo[StateCount];
        }

        [Browsable(false)] // 캔버스상에 보일 상태
        public SGUISpriteInfo VisualSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = 0; i < _sprites.Length; ++i)
                        if (i == _state)
                            return _sprites[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = 0; i < _sprites.Length; ++i)
                    if (i == _state && !_sprites[i].IsNull)
                        return _sprites[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return _sprites.FirstOrDefault(sprite => !sprite.IsNull);
            }
        }

        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(SGUIElement.OrderVisualSize)]
        public override Size VisualSize
        {
            get
            {
                for (int i = 0; i < StateCount; ++i)
                    if (!_sprites[i].IsNull)
                        return _sprites[i].Rect.Size;

                return Constant.DefaultVisualSize;
            }
        }

        private void NotifySpriteChanged()
        {
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
            OnPropertyChanged(nameof(VisualSprite));
            OnPropertyChanged(nameof(VisualSpriteSource));
        }

        [Browsable(false)]
        public BitmapSource VisualSpriteSource => VisualSprite.Source;

        [Category(Constant.ButtonCategoryName), DisplayName("일반"), PropertyOrder(OrderNormal)]
        public SGUISpriteInfo Normal
        {
            get => _sprites[StateNormal];
            set
            {
                _sprites[StateNormal] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

       

        [Category(Constant.ButtonCategoryName), DisplayName("마우스위"), PropertyOrder(OrderOver)]
        public SGUISpriteInfo Over
        {
            get => _sprites[StateOver];
            set
            {
                _sprites[StateOver] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("누름"), PropertyOrder(OrderPressed)]
        public SGUISpriteInfo Pressed
        {
            get => _sprites[StatePressed];
            set
            {
                _sprites[StatePressed] = value;
                OnPropertyChanged();
                NotifySpriteChanged();
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("비활성화"), PropertyOrder(OrderDisabled)]
        public SGUISpriteInfo Disabled
        {
            get => _sprites[StateDisabled];
            set
            {
                _sprites[StateDisabled] = value;
                OnPropertyChanged();
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


        public override SGUIElementType UIElementType => SGUIElementType.Button;
        [Browsable(false)] public override bool Manipulatable => false;

        

        public override object Clone()
        {
            SGUIButton button = new SGUIButton();
            button.CopyFrom(this);

            for (int i = 0; i < StateCount; ++i)
                button._sprites[i] = _sprites[i];

            return button;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            SGUISpriteInfoExt.TryGetSgaImgFileName(in _sprites, out string sga, out string img);
            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = _sprites.ToFullString();
            return root;
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
            StringEx.ParseIntNumberN((string)root[JsonSpriteKey], sprites);
            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in _sprites);
        }

        // 기본적으로 엘리먼트의 이벤트는 "전파"되도록한다.
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

            return false;
        }

        public override void CreateInit() => VisualName = $"버튼_{Seq++}";
        public static int Seq = 0;
        private SGUISpriteInfo[] _sprites;
    }
}
