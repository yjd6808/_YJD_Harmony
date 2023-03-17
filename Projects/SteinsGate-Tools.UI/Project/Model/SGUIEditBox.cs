/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Resource;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    public enum InputMode
    {
        Any,
        EmailAddress,
        Numeric,
        PhoneNumber,
        Url,
        Decimal,
        SingleLine,
    }

    [CategoryOrder(Constant.EditBoxCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIEditBox : SGUIElement
    {
        public const int OrderSize = 1;
        public const int OrderTextHAlign = 2;
        public const int OrderFontSize = 3;
        public const int OrderFontColor = 4;
        public const int OrderPlaceholderText = 5;
        public const int OrderPlaceholderFontColor = 6;
        public const int OrderPlaceholderFontSize = 7;
        public const int OrderMaxLength = 8;
        public const int OrderInputMode = 9;

        public SGUIEditBox()
        {
            _visualSize = Constant.DefaultBoxVisualSize;
            _textHAlign = HAlignment.Right;
            _fontSize = 16;
            _fontColor = Color.FromArgb(255, 0, 0, 0);
            _placeholderText = "텍스트를 입력해주세요.";
            _placeholderFontColor = Color.FromArgb(255, 128, 128, 128);
            _placeholderFontSize = 16;
            _maxLength = 20;
            _inputMode = InputMode.Any;
        }   

        
        [Category(Constant.EditBoxCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override Size VisualSize
        {
            get => new((int)_visualSize.Width, (int)_visualSize.Height);
            set
            {
                _visualSize = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("텍스트 수평정렬"), PropertyOrder(OrderTextHAlign)]
        public HAlignment TextHAlign
        {
            get => _textHAlign;
            set
            {
                _textHAlign = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("폰트 크기"), PropertyOrder(OrderFontSize)]
        public int FontSize
        {
            get => _fontSize;
            set
            {
                _fontSize = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("폰트 색상"), PropertyOrder(OrderFontColor)]
        public Color FontColor
        {
            get => _fontColor;
            set
            {
                _fontColor = value;
                OnPropertyChanged();
            }
        }


        [Category(Constant.EditBoxCategoryName), DisplayName("홀더"), PropertyOrder(OrderPlaceholderText)]
        public string PlaceholderText
        {
            get => _placeholderText;
            set
            {
                _placeholderText = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("홀더 색상"), PropertyOrder(OrderPlaceholderFontColor)]
        public Color PlaceholderFontColor
        {
            get => _placeholderFontColor;
            set
            {
                _placeholderFontColor = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("홀더 크기"), PropertyOrder(OrderPlaceholderFontSize)]
        public int PlaceholderFontSize
        {
            get => _placeholderFontSize;
            set
            {
                _placeholderFontSize = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("입력 길이(최대)"), PropertyOrder(OrderMaxLength)]
        public int MaxLength
        {
            get => _maxLength;
            set
            {
                _maxLength = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("입력 모드"), PropertyOrder(OrderInputMode)]
        public InputMode InputMode
        {
            get => _inputMode;
            set
            {
                _inputMode = value;
                OnPropertyChanged();
            }
        }



        public override SGUIElementType UIElementType => SGUIElementType.EditBox;
        [Browsable(false)] public override bool Manipulatable => true;

        [Browsable(false)]
        public WatermarkTextBox TextBox { get; set; }

        public override object Clone()
        {
            SGUIEditBox editbox = new SGUIEditBox();
            editbox.CopyFrom(this);
            editbox._visualSize = _visualSize;
            editbox._textHAlign = _textHAlign;
            editbox._fontSize = _fontSize;
            editbox._fontColor = _fontColor;
            editbox._placeholderText = _placeholderText;
            editbox._placeholderFontColor = _placeholderFontColor;
            editbox._placeholderFontSize = _placeholderFontSize;
            editbox._inputMode = _inputMode;
            editbox._maxLength = _maxLength;
            return editbox;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            root[JsonTextHAlignKey] = (int)_textHAlign;
            root[JsonFontSizeKey] = _fontSize;
            root[JsonFontColorKey] = _fontColor.ToFullString4B();
            root[JsonPlaceholderTextKey] = _placeholderText;
            root[JsonPlaceholderFontColorKey] = _placeholderFontColor.ToFullString4B();
            root[JsonPlaceholderFontSizeKey] = _placeholderFontSize;
            root[JsonMaxLengthKey] = _maxLength;
            root[JsonInputModeKey] = (int)_inputMode;
            return root;
        }


        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sizeString = (string)root[JsonVisualSizeKey];
            _visualSize = SizeEx.ParseFullString(sizeString);
            _textHAlign = (HAlignment)((int)root[JsonTextHAlignKey]);

            string fontFileName = (string)root[JsonFontKey];
            _fontSize = (int)root[JsonFontSizeKey];

            string fontColorString = (string)root[JsonFontColorKey];
            _fontColor = ColorEx.ParseFullString4B(fontColorString);
            _placeholderText = (string)root[JsonPlaceholderTextKey];

            string placeholderFontColorString = (string)root[JsonPlaceholderFontColorKey];
            _placeholderFontColor = ColorEx.ParseFullString4B(placeholderFontColorString);
            _placeholderFontSize = (int)root[JsonPlaceholderFontSizeKey];
            _maxLength = (int)root[JsonMaxLengthKey];
            _inputMode = (InputMode)(int)root[JsonInputModeKey];
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

            if (ViewModel.IsEventMode)
            {
                Debug.Assert(TextBox != null);
                TextBox.Focus();
            }

            return false;
        }

        public override void CreateInit() => VisualName = $"에딧박스_{Seq++}";
        public static int Seq;

        private Size _visualSize;
        private HAlignment _textHAlign;
        private int _fontSize;
        private Color _fontColor;
        private string _placeholderText;
        private Color _placeholderFontColor;
        private int _placeholderFontSize;
        private int _maxLength;
        private InputMode _inputMode;
    }
}

