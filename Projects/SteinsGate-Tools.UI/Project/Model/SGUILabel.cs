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
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
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

        public SGUILabel()
        {
            _visualSize = Constant.DefaultVisualSize;
        }

        
        [Category(Constant.LabelCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override Size VisualSize
        {
            get => _visualSize;
            set
            {
                _visualSize = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }


        [Browsable(false)]
        public FontFamily FontFamily => R.GetFontFamily(_font);
        [Category(Constant.LabelCategoryName), DisplayName("폰트"), PropertyOrder(OrderFont)]
        public FontType Font
        {
            get => _font;
            set
            {
                _font = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(FontFamily));
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("폰트 크기"), PropertyOrder(OrderFontSize)]
        public int FontSize
        {
            get => _fontSize;
            set
            {
                _fontSize = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("폰트 색상"), PropertyOrder(OrderFontColor)]
        public Color FontColor
        {
            get => _fontColor;
            set
            {
                _fontColor = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트"), PropertyOrder(OrderText)]
        public string Text
        {
            get => _text;
            set
            {
                _text = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트 랩핑"), PropertyOrder(OrderTextWrap)]
        public bool TextWrap
        {
            get => _textWrap;
            set
            {
                _textWrap = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TextWrapEnum));
            }
        }


        [Browsable(false)] 
        public TextWrapping TextWrapEnum => _textWrap ? TextWrapping.Wrap : TextWrapping.NoWrap;



        [Category(Constant.LabelCategoryName), DisplayName("텍스트 수직정렬"), PropertyOrder(OrderTextVAlign)]
        public VAlignment TextVAlign
        {
            get => _textVAlign;
            set
            {
                _textVAlign = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.LabelCategoryName), DisplayName("텍스트 수평정렬"), PropertyOrder(OrderTextHAlign)]
        public HAlignment TextHAlign
        {
            get => _textHAlign;
            set
            {
                _textHAlign = value;
                OnPropertyChanged();
            }
        }



        public override SGUIElementType UIElementType => SGUIElementType.Label;
        [Browsable(false)] public override bool Manipulatable => true;

        public override object Clone()
        {
            SGUILabel label = new SGUILabel();
            label.CopyFrom(this);
            label._visualSize = _visualSize;
            label._font = _font;
            label._fontSize = _fontSize;
            label._fontColor = _fontColor;
            label._text = _text;
            label._textWrap = _textWrap;
            label._textVAlign = _textVAlign;
            label._textHAlign = _textHAlign;
            return label;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            // 인덱스를 뛰어쓰기로 구분해서 돌려줌
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            root[JsonFontKey] = _font + ".ttf";
            root[JsonFontSizeKey] = _fontSize;
            root[JsonFontColorKey] = _fontColor.ToFullString4B();
            root[JsonTextKey] = _text;
            root[JsonTextWrapKey] = _textWrap;
            root[JsonTextVAlignKey] = (int)_textVAlign;
            root[JsonTextHAlignKey] = (int)_textHAlign;

            return root;
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sizeString = (string)root[JsonImgKey];
            _visualSize = SizeEx.ParseFullString(sizeString);
            string fontFileName = (string)root[JsonFontKey];
            _font = (FontType)Enum.Parse(typeof(FontType), Path.GetFileNameWithoutExtension(fontFileName));
            _fontSize = (int)root[JsonFontSizeKey];

            string fontColorString = (string)root[JsonFontColorKey];
            _fontColor = ColorEx.ParseFullString4B(fontColorString);
            _text = (string)root[JsonTextKey];
            _textWrap = (bool)root[JsonTextWrapKey];
            _textVAlign = (VAlignment)((int)root[JsonTextVAlignKey]);
            _textHAlign = (HAlignment)((int)root[JsonTextHAlignKey]);
        }

        public override void CreateInit() => VisualName = $"라벨_{Seq++}";
        public static int Seq;

        private Size _visualSize;
        private FontType _font;
        private int _fontSize;
        private Color _fontColor;
        private string _text;
        private bool _textWrap;
        private VAlignment _textVAlign;
        private HAlignment _textHAlign;
        
        
    }
}

