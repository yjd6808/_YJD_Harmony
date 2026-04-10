/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 1:58:46 PM
 *
 */

using System;
using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
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
        private string text_;
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
        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            root[JsonVisualSizeKey] = visualSize_.ToFullString();
            root[JsonFontKey] = font_ + ".ttf";
            root[JsonFontSizeKey] = fontSize_;
            root[JsonFontColorKey] = fontColor_.ToFullString4B();
            root[JsonTextKey] = text_.Unescape();
            root[JsonTextWrapKey] = textWrap_;
            root[JsonTextVAlignKey] = (int)textVAlign_;
            root[JsonTextHAlignKey] = (int)textHAlign_;

            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseJObject(JObject _root)
        {
            base.ParseJObject(_root);

            string sizeString = (string)_root[JsonVisualSizeKey];
            visualSize_ = SizeEx.ParseFullString(sizeString);
            string fontFileName = (string)_root[JsonFontKey];
            font_ = (FontType)Enum.Parse(typeof(FontType), Path.GetFileNameWithoutExtension(fontFileName));
            fontSize_ = (int)_root[JsonFontSizeKey];

            string fontColorString = (string)_root[JsonFontColorKey];
            fontColor_ = ColorEx.ParseFullString4B(fontColorString);
            text_ = (string)_root[JsonTextKey];
            textWrap_ = (bool)_root[JsonTextWrapKey];
            textVAlign_ = (VAlignment)(int)_root[JsonTextVAlignKey];
            textHAlign_ = (HAlignment)(int)_root[JsonTextHAlignKey];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"라벨_{Seq++}";
    }
}
