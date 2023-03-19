/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 4:55:06 PM
 *
 * 빨리 게임만들고 싶어서 아무 생각없이 만듬.. ㅠㅠ
 * 일단 UI 툴상에서는 눈으로 보이는 모습만 중요하기땜에 스크롤바의 기능 동작 여부는 중요치않다.
 * 따라서 코드는 좀 조잡한데, 문제는 없다.
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.ScrollBarCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIScrollBar : SGUIElement, ISizeRestorable
    {
        public const int IndexUpNormal = 0;
        public const int IndexUpOver = 1;
        public const int IndexDownNormal = 2;
        public const int IndexDownOver = 3;
        public const int IndexThumbNormal = 4;
        public const int IndexThumbOver = 5;
        public const int IndexTrack = 6;

        public const int OrderUpNormal = 1;
        public const int OrderUpOver = 2;
        public const int OrderDownNormal = 3;
        public const int OrderDownOver = 4;
        public const int OrderTrack = 5;
        public const int OrderThumbNormal = 6;
        public const int OrderThumbOver = 7;
        public const int OrderSize = 8;
        public const int OrderTrackSize = 9;



        public SGUIScrollBar()
        {
            _sprites = new SGUISpriteInfo[7];
            _visualSize = Constant.DefaultVisualSize;
        }

        public override SGUIElementType UIElementType => SGUIElementType.ScrollBar;
        public override bool Manipulatable => true;
        public static int Seq = 0;
        public override void CreateInit() => _visualName = $"스크롤바_{Seq++}";



        [Browsable(false)] public double VisualUpHeight => VisualUpSprite.IsNull ? 0 : VisualUpSprite.Height;
        [Browsable(false)] public BitmapSource VisualUpSpriteSource => VisualUpSprite.Source;
        [Browsable(false)]
        public SGUISpriteInfo VisualUpSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = IndexUpNormal; i <= IndexUpOver; ++i)
                        if (i == _upState)
                            return _sprites[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexUpNormal; i <= IndexUpOver; ++i)
                    if (i == _upState && !_sprites[i].IsNull)
                        return _sprites[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로

                return _sprites.Take(new Range(IndexUpNormal, IndexUpOver)).FirstOrDefault(x => !x.IsNull);
            }
        }


        [Browsable(false)] public double VisualDownHeight => VisualDownSprite.IsNull ? 0 : VisualDownSprite.Height;
        [Browsable(false)] public BitmapSource VisualDownSpriteSource => VisualDownSprite.Source;
        [Browsable(false)]
        public SGUISpriteInfo VisualDownSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = IndexDownNormal; i <= IndexDownOver; ++i)
                        if (i == _downState)
                            return _sprites[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexDownNormal; i <= IndexDownOver; ++i)
                    if (i == _downState && !_sprites[i].IsNull)
                        return _sprites[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return _sprites.Take(new Range(IndexDownNormal, IndexDownOver)).FirstOrDefault(x => !x.IsNull);
            }
        }

        [Browsable(false)] public double VisualThumbHeight => VisualThumbSprite.IsNull ? 0 : VisualThumbSprite.Height;
        [Browsable(false)] public BitmapSource VisualThumbSpriteSource => VisualThumbSprite.Source;
        [Browsable(false)]
        public SGUISpriteInfo VisualThumbSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = IndexThumbNormal; i <= IndexThumbOver; ++i)
                        if (i == _thumbState)
                            return _sprites[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexThumbNormal; i <= IndexThumbOver; ++i)
                    if (i == _thumbState && !_sprites[i].IsNull)
                        return _sprites[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return _sprites.Take(new Range(IndexThumbNormal, IndexThumbOver)).FirstOrDefault(x => !x.IsNull);
            }
        }





        // 4개중 제일 너비가 큰걸로 잡음
        private void UpdateVisualWidth()
        {
            _visualSize.Width = MaxWidth;
            _widthInitialized = true;

            if (_visualSize.Width <= 1)
                _visualSize.Width = Constant.DefaultVisualSize.Width;
        }

        private void UpdateVisualHeight() // 내가 강제로 적용하고잫라는 트랙 높이
        {
            _visualSize.Height = MaxHeight;
            _heightInitialized = true;

            if (_visualSize.Height <= 1)
                _visualSize.Height = Constant.DefaultVisualSize.Height;
        }
        [Browsable(false)] public BitmapSource VisualTrackSpriteSource => _sprites[IndexTrack].Source;
        [Category(Constant.ScrollBarCategoryName), DisplayName("트랙"), PropertyOrder(OrderTrack)]
        public SGUISpriteInfo Track
        {
            get => _sprites[IndexTrack];
            set
            {
                _sprites[IndexTrack] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TrackHeight));
                OnPropertyChanged(nameof(VisualTrackSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("위 Normal"), PropertyOrder(OrderUpNormal)]
        public SGUISpriteInfo UpNormal
        {
            get => _sprites[IndexUpNormal];
            set
            {
                _sprites[IndexUpNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualUpSprite));
                OnPropertyChanged(nameof(VisualUpSpriteSource));
                OnPropertyChanged(nameof(VisualUpHeight));
                OnSpriteChanged();

            }
        }

        private void OnSpriteChanged()
        {
            UpdateVisualWidth();
            UpdateVisualHeight();
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("위 Over"), PropertyOrder(OrderUpOver)]
        public SGUISpriteInfo UpOver
        {
            get => _sprites[IndexUpOver];
            set
            {
                _sprites[IndexUpOver] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualUpSprite));
                OnPropertyChanged(nameof(VisualUpHeight));
                OnPropertyChanged(nameof(VisualUpSpriteSource));
                OnSpriteChanged();
            }
        }
        [Category(Constant.ScrollBarCategoryName), DisplayName("아래 Normal"), PropertyOrder(OrderDownNormal)]
        public SGUISpriteInfo DownNormal
        {
            get => _sprites[IndexDownNormal];
            set
            {
                _sprites[IndexDownNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualDownSprite));
                OnPropertyChanged(nameof(VisualDownHeight));
                OnPropertyChanged(nameof(VisualDownSpriteSource));
                OnSpriteChanged();
            }
        }
        [Category(Constant.ScrollBarCategoryName), DisplayName("아래 Over"), PropertyOrder(OrderDownOver)]
        public SGUISpriteInfo DownOver
        {
            get => _sprites[IndexDownOver];
            set
            {
                _sprites[IndexDownOver] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualDownSprite));
                OnPropertyChanged(nameof(VisualDownHeight));
                OnPropertyChanged(nameof(VisualDownSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("바 Normal"), PropertyOrder(OrderThumbNormal)]
        public SGUISpriteInfo ThumbNormal
        {
            get => _sprites[IndexThumbNormal];
            set
            {
                _sprites[IndexThumbNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualThumbSprite));
                OnPropertyChanged(nameof(VisualThumbHeight));
                OnPropertyChanged(nameof(VisualThumbSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("바 Over"), PropertyOrder(OrderThumbOver)]
        public SGUISpriteInfo ThumbOver
        {
            get => _sprites[IndexThumbOver];
            set
            {
                _sprites[IndexThumbOver] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualThumbSprite));
                OnPropertyChanged(nameof(VisualThumbHeight));
                OnPropertyChanged(nameof(VisualThumbSpriteSource));
                OnSpriteChanged();
            }
        }

        public int MaxWidth
        {
            get
            {
                SGUISpriteInfo visualDown = VisualDownSprite;
                SGUISpriteInfo visualUp = VisualUpSprite;
                SGUISpriteInfo visualThumb = VisualThumbSprite;

                if (!_widthInitialized)
                    return MathEx.Max(visualDown.Width, visualUp.Width, visualThumb.Width, _sprites[IndexTrack].Width);

                return MathEx.Max(_visualSize.Width, visualDown.Width, visualUp.Width, visualThumb.Width, _sprites[IndexTrack].Width);
            }
        }

        public int MaxHeight
        {
            get
            {
                SGUISpriteInfo visualDown = VisualDownSprite;
                SGUISpriteInfo visualUp = VisualUpSprite;
                SGUISpriteInfo visualThumb = VisualThumbSprite;

                int height = 0;
                height += visualUp.Height;
                height += MathEx.Max(visualThumb.Height, _sprites[IndexTrack].Height);
                height += visualDown.Height;

                if (!_heightInitialized)
                    return height;

                return Math.Max(_visualSize.Height, height);
            }
        }

        public double TrackHeight
        {
            get
            {
                double trackHeight = _visualSize.Height;

                SGUISpriteInfo visualDown = VisualDownSprite;
                SGUISpriteInfo visualUp = VisualUpSprite;

                if (!visualUp.IsNull)
                    trackHeight -= visualUp.Height;

                if (!visualDown.IsNull)
                    trackHeight -= visualDown.Height;

                return trackHeight;
            }
        }

        [ReadOnly(false)]
        [Category(Constant.ScrollBarCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => _visualSize;
            set
            {
                _visualSize = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TrackSize));
                OnPropertyChanged(nameof(TrackHeight));
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("트랙 크기"), PropertyOrder(OrderTrackSize)]
        public IntSize TrackSize
        {
            get => new(_visualSize.Width, TrackHeight);
            set
            {
                _visualSize.Width = Math.Max(_visualSize.Width, value.Width);
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualRect));
            }
        }


        [Browsable(false)]
        public int UpState
        {
            set
            {
                _upState = value;
                OnPropertyChanged(nameof(VisualUpSpriteSource));
            }
        }

        [Browsable(false)]
        public int DownState
        {
            set
            {
                _downState = value;
                OnPropertyChanged(nameof(VisualDownSpriteSource));
            }
        }

        [Browsable(false)]
        public int ThumbState
        {
            set
            {
                _thumbState = value;
                OnPropertyChanged(nameof(VisualThumbSpriteSource));
            }
        }

        public void RestoreSize()
        {
            _widthInitialized = false;
            _heightInitialized = false;

            UpdateVisualWidth();
            UpdateVisualHeight();

            OnPropertyChanged(nameof(VisualUpHeight));
            OnPropertyChanged(nameof(VisualThumbHeight));
            OnPropertyChanged(nameof(VisualDownHeight));
            OnPropertyChanged(nameof(TrackHeight));
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
        }

        private bool IsVisualUpRectContainPoint(IntPoint p)
        {
            SGUISpriteInfo visualUp = VisualUpSprite;
            if (visualUp.IsNull) return false;
            Rect rc = visualUp.Rect;
            rc.Width = _visualSize.Width;
            return rc.Contains(p);
        }

        private bool IsVisualThumbRectContainPoint(IntPoint p)
        {
            SGUISpriteInfo visualThumb = VisualThumbSprite;
            if (visualThumb.IsNull) return false;

            Rect visualThumbRect = visualThumb.Rect;
            visualThumbRect.Width = _visualSize.Width;

            if (!VisualUpSprite.IsNull)
                visualThumbRect.Y += VisualUpSprite.Height;

            return visualThumbRect.Contains(p);
        }

        private bool IsVisualDownRectContainPoint(IntPoint p)
        {
            SGUISpriteInfo visualDown = VisualDownSprite;
            if (visualDown.IsNull) return false;

            Rect visualDownRect = visualDown.Rect;
            visualDownRect.Width = _visualSize.Width;

            if (!VisualUpSprite.IsNull)
                visualDownRect.Y += VisualUpSprite.Height;

            if (!_sprites[IndexTrack].IsNull)
                visualDownRect.Y += TrackHeight;

            return visualDownRect.Contains(p);
        }

        // 300, 400에 엘리먼트가 위치해있을때
        // 300, 400는 엘리먼트기준 0, 0의 좌표이다.
        public IntPoint ConvertElementPosition(IntPoint p)
            => new(p.X - _visualPosition.X, p.Y - _visualPosition.Y);

        public override bool OnMouseMove(IntPoint p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool elementContained = ContainPoint(p);


            if (!elementContained)
            {
                State = StateNormal;
                UpState = IndexUpNormal;
                DownState = IndexDownNormal;
                ThumbState = IndexThumbNormal;
                return true;
            }

            State = StateOver;

            p = ConvertElementPosition(p);

            UpState = IsVisualUpRectContainPoint(p) ? IndexUpOver : IndexUpNormal;
            DownState = IsVisualDownRectContainPoint(p) ? IndexDownOver : IndexDownNormal;
            ThumbState = IsVisualThumbRectContainPoint(p) ? IndexThumbOver : IndexThumbNormal;

            return false;
        }

        public override bool OnMouseDown(IntPoint p)
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

        public override bool OnMouseUp(IntPoint p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(p);
            State = StateNormal;
            UpState = IndexUpNormal;
            DownState = IndexDownNormal;
            ThumbState = IndexThumbNormal;


            if (!contained)
                return true;

            return false;
        }



        public override object Clone()
        {
            SGUIScrollBar scrollbar = new SGUIScrollBar();
            scrollbar.CopyFrom(this);
            scrollbar._visualSize = VisualSize;
            Array.Copy(_sprites, scrollbar._sprites, _sprites.Length);
            //scrollbar.UpdateVisualWidth();
            return scrollbar;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            SGUISpriteInfoExt.TryGetSgaImgFileName(in _sprites, out string sga, out string img);
            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = _sprites.ToFullString();
            root[JsonTrackSizeKey] = TrackSize.ToFullString();
            return root;
        }


        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sgaName = (string)root[JsonSgaKey];

            if (sgaName == string.Empty)
                return;

            string imgName = (string)root[JsonImgKey];
            string trackSizeString = (string)root[JsonTrackSizeKey];

            IntSize trackSize = SizeEx.ParseFullString(trackSizeString);

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int[] sprites = new int[7];
            StringEx.ParseIntNumberN((string)root[JsonSpriteKey], sprites);
            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in _sprites);

            int height = 0;

            if (!VisualUpSprite.IsNull)
                height += VisualUpSprite.Height;

            height += trackSize.Height;

            if (!VisualDownSprite.IsNull)
                height += VisualDownSprite.Height;


            _visualSize.Width = trackSize.Width;
            _visualSize.Height = Math.Max(height, MaxHeight);
        }

        // 실제 기입되는 중요한 데이터
        private SGUISpriteInfo[] _sprites;

        // 이벤트 처리를 위한 데이터
        private int _upState;
        private int _thumbState;
        private int _downState;
        private IntSize _visualSize;
        private bool _widthInitialized;
        private bool _heightInitialized;

    }
}
