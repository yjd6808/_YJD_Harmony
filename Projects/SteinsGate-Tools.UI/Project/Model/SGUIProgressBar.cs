/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 1:13:24 PM
 *
 */

using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Diagnostics;
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
using Vanara.PInvoke;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using static Vanara.PInvoke.Kernel32;

namespace SGToolsUI.Model
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


        public SGUIProgressBar()
        {
            _direction = ProgressIncreaseDirection.LeftRight;
            _percent = 70.0;
            _visualSize = Constant.DefaultVisualSize;
        }



        [Category(Constant.ProgressBarCategoryName), DisplayName("퍼센트"), PropertyOrder(OrderPercent)]
        public double Percent
        {
            get => _percent;
            set
            {
                if (value < 0)
                    value = 0;

                else if (value > 100.0)
                    value = 100;

                _percent = value;

                OnPropertyChanged();
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
            }
        }

        [Category(Constant.ProgressBarCategoryName), DisplayName("방향"), PropertyOrder(OrderDirection)]
        public ProgressIncreaseDirection Direction
        {
            get => _direction;
            set
            {
                _direction = value;

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
                if (_direction == ProgressIncreaseDirection.LeftRight ||
                    _direction == ProgressIncreaseDirection.RightLeft)
                {
                    double width = _visualSize.Width * (_percent / 100.0);
                    return width;
                }

                return _visualSize.Width;
            }
        }

        [Browsable(false)] public double PercentHeight
        // 수직방향시에 보일 너비 비율
        {
            get
            {
                if (_direction == ProgressIncreaseDirection.TopBottom ||
                    _direction == ProgressIncreaseDirection.BottomTop)
                {
                    double height = _visualSize.Height * (_percent / 100.0);
                    return height;
                } 
                return _visualSize.Height;
            }
        }

        [Browsable(false)]
        public HorizontalAlignment PercentHAlignment
        {
            get
            {
                if (_direction == ProgressIncreaseDirection.LeftRight)
                    return System.Windows.HorizontalAlignment.Left;
                if (_direction == ProgressIncreaseDirection.RightLeft)
                    return System.Windows.HorizontalAlignment.Right;
                return System.Windows.HorizontalAlignment.Left;
            }
        }
        [Browsable(false)]
        public VerticalAlignment PercentVAlignment
        {
            get
            {
                if (_direction == ProgressIncreaseDirection.TopBottom)
                    return System.Windows.VerticalAlignment.Top;
                if (_direction == ProgressIncreaseDirection.BottomTop)
                    return System.Windows.VerticalAlignment.Bottom;
                return System.Windows.VerticalAlignment.Top;
            }
        }

        [Browsable(false)] public double CanvasLeft  => _direction == ProgressIncreaseDirection.LeftRight ? 0.0 : Double.NaN;
        [Browsable(false)] public double CanvasRight => _direction == ProgressIncreaseDirection.RightLeft ? 0.0 : Double.NaN;
        [Browsable(false)] public double CanvasTop => _direction == ProgressIncreaseDirection.TopBottom ? 0.0 : Double.NaN;
        [Browsable(false)] public double CanvasBottom => _direction == ProgressIncreaseDirection.BottomTop ? 0.0 : Double.NaN;





        [ReadOnly(false)]
        [Category(Constant.ProgressBarCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override Size VisualSize
        {
            get => new((int)_visualSize.Width, (int)_visualSize.Height);
            set
            {
                _visualSize = value;
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
                if (_sprite.IsNull) return 1.0;
                return _visualSize.Width / _sprite.Sprite.Width;
            }
        }

        [Browsable(false)]
        public double ScaleY
        {
            get
            {
                if (_sprite.IsNull) return 1.0;
                return _visualSize.Height / _sprite.Sprite.Height;
            }
        }


      

        [Browsable(false)]
        public BitmapSource SpriteSource => Sprite.Source;

        [Category(Constant.ProgressBarCategoryName), DisplayName("스프라이트"), PropertyOrder(OrderSprite)]
        public SGUISpriteInfo Sprite
        {
            get => _sprite;
            set
            {
                _sprite = value;

                if (!_sprite.IsNull)
                    _visualSize = _sprite.Sprite.Rect.Size;
                else
                    _visualSize = Constant.DefaultVisualSize;

                OnPropertyChanged();
                OnPropertyChanged(nameof(PercentWidth));
                OnPropertyChanged(nameof(PercentHeight));
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualRect));
                OnPropertyChanged(nameof(SpriteSource));
            }
        }


        public override SGUIElementType UIElementType => SGUIElementType.ProgressBar;
        public override bool Manipulatable => true;
        public static int Seq = 0;
        public override void CreateInit() => VisualName = $"프로그래스바_{Seq++}";

        public override object Clone()
        {
            SGUIProgressBar progressBar = new SGUIProgressBar();
            progressBar.CopyFrom(this);
            progressBar._sprite = _sprite;
            progressBar._visualSize = _visualSize;
            progressBar._percent = _percent;
            progressBar._direction = _direction;
            return progressBar;
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
            root[JsonSpriteKey] = _sprite.SpriteIndex.ToString();
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            root[JsonDirectionKey] = (int)_direction;
            return root;
        }

        private void GetSgaImgFileName(out string sga, out string img)
        {
            sga = string.Empty;
            img = string.Empty;

            if (!_sprite.IsNull)
            {
                sga = _sprite.Sga.FileNameWithoutExt;
                img = _sprite.Img.Header.NameWithoutExt;
            }
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sgaName = (string)root[JsonSgaKey];

            if (sgaName == string.Empty)
                return;

            string imgName = (string)root[JsonImgKey];
            string sizeString = (string)root[JsonVisualSizeKey];

            _visualSize = SizeEx.ParseFullString(sizeString);

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int spriteIndex = int.Parse((string)root[JsonSpriteKey]);

            if (spriteIndex == Constant.InvalidValue)
                return;

            SgaSprite sprite = img.GetSprite(spriteIndex) as SgaSprite;
            if (sprite == null)
                throw new Exception($"{sgaName} -> {imgName} -> {spriteIndex}가 SgaSprite 타입이 아닙니다.");
            _sprite = new SGUISpriteInfo(sga, img, sprite);
            _percent = 70.0;
            _direction = (ProgressIncreaseDirection)(int)root[JsonDirectionKey];
        }


        private double _percent;
        private SGUISpriteInfo _sprite;
        private Size _visualSize;
        private ProgressIncreaseDirection _direction;
    }
}
