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
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    
    [CategoryOrder(Constant.ButtonCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIButton : SGUIElement
    {
        public const int StateCount = 4;
        public const int StateNormal = 0;
        public const int StatePressed = 1;
        public const int StateOver = 2;
        public const int StateDisabled = 3;

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
                // 현재 상태를 우선해서 보여준다.
                for (int i = 0; i < _sprites.Length; ++i)
                    if (i == _state && !_sprites[i].IsNull)
                        return _sprites[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return _sprites.FirstOrDefault(sprite => !sprite.IsNull);
            }
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
                DetermineSize();
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSprite));
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("누름"), PropertyOrder(OrderPressed)]
        public SGUISpriteInfo Pressed
        {
            get => _sprites[StatePressed];
            set
            {
                _sprites[StatePressed] = value;
                DetermineSize();
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSprite));
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("마우스위"), PropertyOrder(OrderOver)]
        public SGUISpriteInfo Over
        {
            get => _sprites[StateOver];
            set
            {
                _sprites[StateOver] = value;
                DetermineSize();
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSprite));
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }

        [Category(Constant.ButtonCategoryName), DisplayName("비활성화"), PropertyOrder(OrderDisabled)]
        public SGUISpriteInfo Disabled
        {
            get => _sprites[StateDisabled];
            set
            {
                _sprites[StateDisabled] = value;
                DetermineSize();
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSprite));
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }


        // 어떤 버튼 상태의 이미지 크기로 비주얼 사이즈를 업데이트할지 결정
        private void DetermineSize()
        {
            for (int i = 0; i < StateCount; ++i)
                if (!_sprites[i].IsNull)
                {
                    VisualRect = _sprites[i].Rect;
                    return;
                }
        }


        [Browsable(false)]
        public int State
        {
            get => _state;
            set
            {
                _state = value;

                // 상태가 변했지만 스프라이트 정보가 주입안되어있으면 업데이트 불가능
                if (_sprites[value].IsNull)
                    return;

                OnPropertyChanged(nameof(VisualSprite));
                OnPropertyChanged(nameof(VisualSpriteSource));
            }
        }


        public override SGUIElementType UIElementType => SGUIElementType.Button;

        public override void CreateInit()
        {
            VisualName = $"버튼_{Seq++}";
        }

        public override object Clone()
        {
            SGUIButton button = new SGUIButton();
            button.CopyFrom(this);

            for (int i = 0; i < StateCount; ++i)
                button._sprites[i] = _sprites[i];

            return button;
        }

        public static int Seq = 0;
        private SGUISpriteInfo[] _sprites;
        private int _state = StateNormal;
    }
}
