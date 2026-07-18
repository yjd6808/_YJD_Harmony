/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System.ComponentModel;
using System.Diagnostics;
using System.Xml.Linq;
using System.Windows.Media;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using Xceed.Wpf.Toolkit;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
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

        public static int Seq;

        private HAlignment textHAlign_;
        private int fontSize_;
        private Color fontColor_;
        private string placeholderText_;
        private Color placeholderFontColor_;
        private int placeholderFontSize_;
        private int maxLength_;
        private InputMode inputMode_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIEditBox()
        {
            visualSize_ = Constant.DefaultBoxVisualSize;
            textHAlign_ = HAlignment.Right;
            fontSize_ = 16;
            fontColor_ = Color.FromArgb(255, 0, 0, 0);
            placeholderText_ = "텍스트를 입력해주세요.";
            placeholderFontColor_ = Color.FromArgb(255, 128, 128, 128);
            placeholderFontSize_ = 16;
            maxLength_ = 20;
            inputMode_ = InputMode.Any;
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("텍스트 수평정렬"), PropertyOrder(OrderTextHAlign)]
        public HAlignment TextHAlign
        {
            get => textHAlign_;
            set
            {
                textHAlign_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("폰트 크기"), PropertyOrder(OrderFontSize)]
        public int FontSize
        {
            get => fontSize_;
            set
            {
                fontSize_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("폰트 색상"), PropertyOrder(OrderFontColor)]
        public Color FontColor
        {
            get => fontColor_;
            set
            {
                fontColor_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("홀더"), PropertyOrder(OrderPlaceholderText)]
        public string PlaceholderText
        {
            get => placeholderText_;
            set
            {
                placeholderText_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("홀더 색상"), PropertyOrder(OrderPlaceholderFontColor)]
        public Color PlaceholderFontColor
        {
            get => placeholderFontColor_;
            set
            {
                placeholderFontColor_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("홀더 크기"), PropertyOrder(OrderPlaceholderFontSize)]
        public int PlaceholderFontSize
        {
            get => placeholderFontSize_;
            set
            {
                placeholderFontSize_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("입력 길이(최대)"), PropertyOrder(OrderMaxLength)]
        public int MaxLength
        {
            get => maxLength_;
            set
            {
                maxLength_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.EditBoxCategoryName), DisplayName("입력 모드"), PropertyOrder(OrderInputMode)]
        public InputMode InputMode
        {
            get => inputMode_;
            set
            {
                inputMode_ = value;
                OnPropertyChanged();
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.EditBox;
        [Browsable(false)] public override bool Manipulatable => true;

        [Browsable(false)]
        public WatermarkTextBox? TextBox { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "EditBox";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            root.SetAttributeValue("width", visualSize_.Width);
            root.SetAttributeValue("height", visualSize_.Height);
            root.SetAttributeValue("text_halign", (int)textHAlign_);
            root.SetAttributeValue("font_size", fontSize_);
            root.SetAttributeValue("font_color", fontColor_.ToFullString4B());
            root.SetAttributeValue("p_text", placeholderText_);
            root.SetAttributeValue("p_font_color", placeholderFontColor_.ToFullString4B());
            root.SetAttributeValue("p_font_size", placeholderFontSize_);
            root.SetAttributeValue("max_length", maxLength_);
            root.SetAttributeValue("input_mode", (int)inputMode_);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            base.ParseXElement(_root);

            textHAlign_ = (HAlignment)(int)_root.Attribute("text_halign")!;

            fontSize_ = (int)_root.Attribute("font_size")!;

            string fontColorString = (string)_root.Attribute("font_color")!;
            fontColor_ = ColorEx.ParseFullString4B(fontColorString);
            placeholderText_ = (string)_root.Attribute("p_text") ?? string.Empty;

            string placeholderFontColorString = (string)_root.Attribute("p_font_color")!;
            placeholderFontColor_ = ColorEx.ParseFullString4B(placeholderFontColorString);
            placeholderFontSize_ = (int)_root.Attribute("p_font_size")!;
            maxLength_ = (int)_root.Attribute("max_length")!;
            inputMode_ = (InputMode)(int)_root.Attribute("input_mode")!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIEditBox editbox = new SGUIEditBox();
            editbox.CopyFrom(this);
            editbox.visualSize_ = visualSize_;
            editbox.textHAlign_ = textHAlign_;
            editbox.fontSize_ = fontSize_;
            editbox.fontColor_ = fontColor_;
            editbox.placeholderText_ = placeholderText_;
            editbox.placeholderFontColor_ = placeholderFontColor_;
            editbox.placeholderFontSize_ = placeholderFontSize_;
            editbox.inputMode_ = inputMode_;
            editbox.maxLength_ = maxLength_;
            return editbox;
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

            if (ViewModel.IsEventMode)
            {
                Debug.Assert(TextBox != null);
                TextBox.Focus();
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"에딧박스_{Seq++}";
    }
}
