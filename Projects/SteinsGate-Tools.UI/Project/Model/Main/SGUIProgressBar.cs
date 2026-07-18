/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 1:13:24 PM
 *
 */

using System;
using System.ComponentModel;
using System.Xml.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    public enum ProgressIncreaseDirection
    {
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop
    }

    [CategoryOrder(Constant.ProgressBarCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIProgressBar : SGUIElement
    {
        public const int OrderSprite = 1;
        public const int OrderSize = 2;
        public const int OrderPercent = 3;
        public const int OrderDirection = 4;

        public static int Seq = 0;

        private double percent_;
        private SGUISpriteInfo sprite_;
        private ProgressIncreaseDirection direction_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIProgressBar()
        {
            direction_ = ProgressIncreaseDirection.LeftRight;
            percent_ = 70.0;
        }

        [Category(Constant.ProgressBarCategoryName), DisplayName("퍼센트"), PropertyOrder(OrderPercent)]
        public double Percent
        {
            get => percent_;
            set
            {
                if (value < 0)
                    value = 0;
                else if (value > 100.0)
                    value = 100;

                percent_ = value;

                OnPropertyChanged();
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
            }
        }

        [Category(Constant.ProgressBarCategoryName), DisplayName("방향"), PropertyOrder(OrderDirection)]
        public ProgressIncreaseDirection Direction
        {
            get => direction_;
            set
            {
                direction_ = value;

                OnPropertyChanged(nameof(CanvasRight));
                OnPropertyChanged(nameof(CanvasLeft));
                OnPropertyChanged(nameof(CanvasTop));
                OnPropertyChanged(nameof(CanvasBottom));
                OnPropertyChanged(nameof(PercentVAlignment));
                OnPropertyChanged(nameof(PercentHAlignment));
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
            }
        }

        // 컨버터 만들기 귀찮아서 그냥 프로퍼티로 박음
        // UIProgressBarCanvasTemplate.xaml에서 프로그래스 느낌을 주는 용도로 사용하는 프로퍼티 4종
        [Browsable(false)]
        public double PercentWidth
        {
            get
            {
                // 수평방향시에 보일 너비 비율
                if (direction_ == ProgressIncreaseDirection.LeftRight ||
                    direction_ == ProgressIncreaseDirection.RightLeft)
                {
                    double width = visualSize_.Width * (percent_ / 100.0);
                    return width;
                }

                return visualSize_.Width;
            }
        }

        // 수직방향시에 보일 너비 비율
        [Browsable(false)]
        public double PercentHeight
        {
            get
            {
                if (direction_ == ProgressIncreaseDirection.TopBottom ||
                    direction_ == ProgressIncreaseDirection.BottomTop)
                {
                    double height = visualSize_.Height * (percent_ / 100.0);
                    return height;
                }

                return visualSize_.Height;
            }
        }

        [Browsable(false)]
        public System.Windows.HorizontalAlignment PercentHAlignment
        {
            get
            {
                if (direction_ == ProgressIncreaseDirection.LeftRight)
                    return System.Windows.HorizontalAlignment.Left;
                if (direction_ == ProgressIncreaseDirection.RightLeft)
                    return System.Windows.HorizontalAlignment.Right;
                return System.Windows.HorizontalAlignment.Left;
            }
        }

        [Browsable(false)]
        public System.Windows.VerticalAlignment PercentVAlignment
        {
            get
            {
                if (direction_ == ProgressIncreaseDirection.TopBottom)
                    return System.Windows.VerticalAlignment.Top;
                if (direction_ == ProgressIncreaseDirection.BottomTop)
                    return System.Windows.VerticalAlignment.Bottom;
                return System.Windows.VerticalAlignment.Top;
            }
        }

        // 디폴트 값이 NaN이라 하나를 0으로 세팅하면 나머지는 NaN으로 바꿔줘야함
        [Browsable(false)] public double CanvasLeft => direction_ == ProgressIncreaseDirection.LeftRight ? 0.0 : double.NaN;
        [Browsable(false)] public double CanvasRight => direction_ == ProgressIncreaseDirection.RightLeft ? 0.0 : double.NaN;
        [Browsable(false)] public double CanvasTop => direction_ == ProgressIncreaseDirection.TopBottom ? 0.0 : double.NaN;
        [Browsable(false)] public double CanvasBottom => direction_ == ProgressIncreaseDirection.BottomTop ? 0.0 : double.NaN;

        [ReadOnly(false)]
        [Category(Constant.ProgressBarCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => visualSize_;
            set
            {
                visualSize_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(ScaleX));
                OnPropertyChanged(nameof(ScaleY));
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Browsable(false)]
        public double ScaleX
        {
            get
            {
                if (sprite_.IsNull) return 1.0;
                return (double)visualSize_.Width / sprite_.Sprite!.Width;
            }
        }

        [Browsable(false)]
        public double ScaleY
        {
            get
            {
                if (sprite_.IsNull) return 1.0;
                return (double)visualSize_.Height / sprite_.Sprite!.Height;
            }
        }

        [Browsable(false)]
        public BitmapSource SpriteSource => Sprite.Source;

        [Category(Constant.ProgressBarCategoryName), DisplayName("스프라이트"), PropertyOrder(OrderSprite)]
        public SGUISpriteInfo Sprite
        {
            get => sprite_;
            set
            {
                sprite_ = value;

                if (!sprite_.IsNull)
                    visualSize_ = sprite_.Sprite!.Rect.Size;
                else
                    visualSize_ = Constant.DefaultVisualSize;

                OnPropertyChanged();
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualRect));
                OnPropertyChanged(nameof(SpriteSource));
            }
        }

        public override SGUIElementType UIElementType => SGUIElementType.ProgressBar;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "ProgressBar";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            SGUISpriteInfoExt.TryGetSgaImgFileName(sprite_, out string sga, out string img);

            root.SetAttributeValue("sga", sga);
            root.SetAttributeValue("img", img);
            root.SetAttributeValue("sprite", sprite_.SpriteIndex);
            root.SetAttributeValue("width", visualSize_.Width);
            root.SetAttributeValue("height", visualSize_.Height);
            root.SetAttributeValue("direction", (int)direction_);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            string sgaName = (string)_root.Attribute("sga") ?? string.Empty;

            if (sgaName == string.Empty)
            {
                base.ParseXElement(_root);
                return;
            }

            string imgName = (string)_root.Attribute("img")!;

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int spriteIndex = (int)_root.Attribute("sprite")!;

            if (spriteIndex == Constant.InvalidValue)
            {
                base.ParseXElement(_root);
                return;
            }

            SgaSprite? sprite = img.GetSprite(spriteIndex) as SgaSprite;
            if (sprite == null)
                throw new Exception($"{sgaName} -> {imgName} -> {spriteIndex}가 SgaSprite 타입이 아닙니다.");
            sprite_ = new SGUISpriteInfo(sga, img, sprite);
            percent_ = 70.0;
            direction_ = (ProgressIncreaseDirection)(int)_root.Attribute("direction")!;

            base.ParseXElement(_root);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIProgressBar progressBar = new SGUIProgressBar();
            progressBar.CopyFrom(this);
            progressBar.sprite_ = sprite_;
            progressBar.visualSize_ = visualSize_;
            progressBar.percent_ = percent_;
            progressBar.direction_ = direction_;
            return progressBar;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"프로그래스바_{Seq++}";
    }
}
