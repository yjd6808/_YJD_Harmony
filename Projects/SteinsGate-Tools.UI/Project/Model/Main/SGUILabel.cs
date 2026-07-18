/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.IO;
using System.Xml.Linq;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Resource;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.LabelCategoryName, Constant.OtherCategoryOrder)]
    public class SGUILabel : SGUIElement
    {
        public const int OrderSize = 1;
        public const int OrderFont = 2;
        public const int OrderFontSize = 3;
        public const int OrderFontColor = 4;
        public const int OrderText = 5;
        public const int OrderTextWrap = 6;
        public const int OrderTextHAlign = 7;
        public const int OrderTextVAlign = 8;

        public static int Seq;

        private IntSize visualSize_;
        private FontType font_;
        private int fontSize_;
        private Color fontColor_;
        private string text_ = string.Empty;
        private bool textWrap_;
        private VAlignment textVAlign_;
        private HAlignment textHAlign_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUILabel()
        {
            visualSize_ = Constant.DefaultVisualSize;
            fontSize_ = 12;
            font_ = FontType.DF;
            fontColor_ = Brushes.Black.Color;
        }

        [ReadOnly(false)]
        [Category(Constant.LabelCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => visualSize_;
            set
            {
                visualSize_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Browsable(false)]
        public FontFamily FontFamily => R.GetFontFamily(font_);

        [Category(Constant.LabelCategoryName), DisplayName("폰트"), PropertyOrder(OrderFont)]
        public FontType Font
        {
            get => font_;
            set
            {
                font_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(FontFamily));
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("폰트 크기"), PropertyOrder(OrderFontSize)]
        public int FontSize
        {
            get => fontSize_;
            set
            {
                fontSize_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("폰트 색상"), PropertyOrder(OrderFontColor)]
        public Color FontColor
        {
            get => fontColor_;
            set
            {
                fontColor_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트"), PropertyOrder(OrderText)]
        public string Text
        {
            get => text_;
            set
            {
                text_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트 랩핑"), PropertyOrder(OrderTextWrap)]
        public bool TextWrap
        {
            get => textWrap_;
            set
            {
                textWrap_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TextWrapEnum));
            }
        }

        [Browsable(false)]
        public TextWrapping TextWrapEnum => textWrap_ ? TextWrapping.Wrap : TextWrapping.NoWrap;

        [Category(Constant.LabelCategoryName), DisplayName("텍스트 수직정렬"), PropertyOrder(OrderTextVAlign)]
        public VAlignment TextVAlign
        {
            get => textVAlign_;
            set
            {
                textVAlign_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트 수평정렬"), PropertyOrder(OrderTextHAlign)]
        public HAlignment TextHAlign
        {
            get => textHAlign_;
            set
            {
                textHAlign_ = value;
                OnPropertyChanged();
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.Label;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "Label";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            root.SetAttributeValue("width", visualSize_.Width);
            root.SetAttributeValue("height", visualSize_.Height);
            root.SetAttributeValue("font", font_ + ".ttf");
            root.SetAttributeValue("font_size", fontSize_);
            root.SetAttributeValue("font_color", fontColor_.ToFullString4B());
            root.SetAttributeValue("text", text_.Unescape());
            if (textWrap_)
                root.SetAttributeValue("text_wrap", true);
            root.SetAttributeValue("text_valign", (int)textVAlign_);
            root.SetAttributeValue("text_halign", (int)textHAlign_);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            base.ParseXElement(_root);

            visualSize_.Width = (int)_root.Attribute("width")!;
            visualSize_.Height = (int)_root.Attribute("height")!;
            string fontFileName = (string)_root.Attribute("font")!;
            font_ = (FontType)Enum.Parse(typeof(FontType), Path.GetFileNameWithoutExtension(fontFileName));
            fontSize_ = (int)_root.Attribute("font_size")!;

            string fontColorString = (string)_root.Attribute("font_color")!;
            fontColor_ = ColorEx.ParseFullString4B(fontColorString);
            text_ = (string)_root.Attribute("text") ?? string.Empty;
            XAttribute textWrapAttr = _root.Attribute("text_wrap");
            if (textWrapAttr != null)
                textWrap_ = (bool)textWrapAttr;
            textVAlign_ = (VAlignment)(int)_root.Attribute("text_valign")!;
            textHAlign_ = (HAlignment)(int)_root.Attribute("text_halign")!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUILabel label = new SGUILabel();
            label.CopyFrom(this);
            label.visualSize_ = visualSize_;
            label.font_ = font_;
            label.fontSize_ = fontSize_;
            label.fontColor_ = fontColor_;
            label.text_ = text_;
            label.textWrap_ = textWrap_;
            label.textVAlign_ = textVAlign_;
            label.textHAlign_ = textHAlign_;
            return label;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"라벨_{Seq++}";
    }
}
