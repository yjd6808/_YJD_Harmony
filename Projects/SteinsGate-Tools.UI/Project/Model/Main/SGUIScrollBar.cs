/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 4:55:06 PM
 *
 * 빨리 게임만들고 싶어서 아무 생각없이 만듬.. ㅠㅠ
 * 일단 UI 툴상에서는 눈으로 보이는 모습만 중요하기땜에 스크롤바의 기능 동작 여부는 중요치않다.
 * 따라서 코드는 좀 조잡한데, 문제는 없다.
 */

using System;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.ScrollBarCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIScrollBar : SGUIElement, ISizeRestorable
    {
        public const int TextureCount = 7;
        public const int IndexUpNormal = 0;
        public const int IndexUpPressed = 1;
        public const int IndexDownNormal = 2;
        public const int IndexDownPressed = 3;
        public const int IndexThumbNormal = 4;
        public const int IndexThumbPressed = 5;
        public const int IndexTrack = 6;

        public const int OrderUpNormal = 1;
        public const int OrderUpPressed = 2;
        public const int OrderDownNormal = 3;
        public const int OrderDownPressed = 4;
        public const int OrderTrack = 5;
        public const int OrderThumbNormal = 6;
        public const int OrderThumbPressed = 7;
        public const int OrderSize = 8;
        public const int OrderTrackSize = 9;

        public static int Seq = 0;

        // 실제 기입되는 중요한 데이터
        private SGUISpriteInfo[] sprites_;

        // 이벤트 처리를 위한 데이터
        private int upState_;
        private int thumbState_;
        private int downState_;
        private IntSize visualSize_;
        private bool widthInitialized_;
        private bool heightInitialized_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIScrollBar()
        {
            sprites_ = new SGUISpriteInfo[TextureCount];
            visualSize_ = Constant.DefaultVisualSize;
        }

        public override SGUIElementType UIElementType => SGUIElementType.ScrollBar;
        public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => visualName_ = $"스크롤바_{Seq++}";

        [Browsable(false)] public double VisualUpHeight => VisualUpSprite.IsNull ? 0 : VisualUpSprite.Height;
        [Browsable(false)] public BitmapSource VisualUpSpriteSource => VisualUpSprite.Source;

        [Browsable(false)]
        public SGUISpriteInfo VisualUpSprite
        {
            get
            {
                if (ViewModel.IsEventMode)
                    for (int i = IndexUpNormal; i <= IndexUpPressed; ++i)
                        if (i == upState_)
                            return sprites_[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexUpNormal; i <= IndexUpPressed; ++i)
                    if (i == upState_ && !sprites_[i].IsNull)
                        return sprites_[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return sprites_.Take(new Range(IndexUpNormal, IndexUpPressed)).FirstOrDefault(x => !x.IsNull);
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
                    for (int i = IndexDownNormal; i <= IndexDownPressed; ++i)
                        if (i == downState_)
                            return sprites_[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexDownNormal; i <= IndexDownPressed; ++i)
                    if (i == downState_ && !sprites_[i].IsNull)
                        return sprites_[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return sprites_.Take(new Range(IndexDownNormal, IndexDownPressed)).FirstOrDefault(x => !x.IsNull);
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
                    for (int i = IndexThumbNormal; i <= IndexThumbPressed; ++i)
                        if (i == thumbState_)
                            return sprites_[i];

                // 현재 상태를 우선해서 보여준다.
                for (int i = IndexThumbNormal; i <= IndexThumbPressed; ++i)
                    if (i == thumbState_ && !sprites_[i].IsNull)
                        return sprites_[i];

                // 현재 상태에 맞는 스프라이트가 세팅되어있지 않으면 Order순서대로 확인해서 스프라이트 세팅된 놈으로
                return sprites_.Take(new Range(IndexThumbNormal, IndexThumbPressed)).FirstOrDefault(x => !x.IsNull);
            }
        }

        // 4개중 제일 너비가 큰걸로 잡음
        //////////////////////////////////////////////////////////////////////////////////
        private void UpdateVisualWidth()
        {
            visualSize_.Width = MaxWidth;
            widthInitialized_ = true;

            if (visualSize_.Width <= 1)
                visualSize_.Width = Constant.DefaultVisualSize.Width;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void UpdateVisualHeight() // 내가 강제로 적용하고잫라는 트랙 높이
        {
            visualSize_.Height = MaxHeight;
            heightInitialized_ = true;

            if (visualSize_.Height <= 1)
                visualSize_.Height = Constant.DefaultVisualSize.Height;
        }

        [Browsable(false)] public BitmapSource VisualTrackSpriteSource => sprites_[IndexTrack].Source;

        [Category(Constant.ScrollBarCategoryName), DisplayName("트랙"), PropertyOrder(OrderTrack)]
        public SGUISpriteInfo Track
        {
            get => sprites_[IndexTrack];
            set
            {
                sprites_[IndexTrack] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TrackHeight));
                OnPropertyChanged(nameof(VisualTrackSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("위 Normal"), PropertyOrder(OrderUpNormal)]
        public SGUISpriteInfo UpNormal
        {
            get => sprites_[IndexUpNormal];
            set
            {
                sprites_[IndexUpNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualUpSprite));
                OnPropertyChanged(nameof(VisualUpSpriteSource));
                OnPropertyChanged(nameof(VisualUpHeight));
                OnSpriteChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnSpriteChanged()
        {
            UpdateVisualWidth();
            UpdateVisualHeight();
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("위 Pressed"), PropertyOrder(OrderUpPressed)]
        public SGUISpriteInfo UpPressed
        {
            get => sprites_[IndexUpPressed];
            set
            {
                sprites_[IndexUpPressed] = value;
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
            get => sprites_[IndexDownNormal];
            set
            {
                sprites_[IndexDownNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualDownSprite));
                OnPropertyChanged(nameof(VisualDownHeight));
                OnPropertyChanged(nameof(VisualDownSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("아래 Pressed"), PropertyOrder(OrderDownPressed)]
        public SGUISpriteInfo DownPressed
        {
            get => sprites_[IndexDownPressed];
            set
            {
                sprites_[IndexDownPressed] = value;
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
            get => sprites_[IndexThumbNormal];
            set
            {
                sprites_[IndexThumbNormal] = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualThumbSprite));
                OnPropertyChanged(nameof(VisualThumbHeight));
                OnPropertyChanged(nameof(VisualThumbSpriteSource));
                OnSpriteChanged();
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("바 Pressed"), PropertyOrder(OrderThumbPressed)]
        public SGUISpriteInfo ThumbPressed
        {
            get => sprites_[IndexThumbPressed];
            set
            {
                sprites_[IndexThumbPressed] = value;
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

                if (!widthInitialized_)
                    return MathEx.Max(visualDown.Width, visualUp.Width, visualThumb.Width, sprites_[IndexTrack].Width);

                return MathEx.Max(visualSize_.Width, visualDown.Width, visualUp.Width, visualThumb.Width, sprites_[IndexTrack].Width);
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
                height += MathEx.Max(visualThumb.Height, sprites_[IndexTrack].Height);
                height += visualDown.Height;

                if (!heightInitialized_)
                    return height;

                return Math.Max(visualSize_.Height, height);
            }
        }

        public double TrackHeight
        {
            get
            {
                double trackHeight = visualSize_.Height;

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
            get => visualSize_;
            set
            {
                visualSize_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(TrackSize));
                OnPropertyChanged(nameof(TrackHeight));
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category(Constant.ScrollBarCategoryName), DisplayName("트랙 크기"), PropertyOrder(OrderTrackSize)]
        public IntSize TrackSize
        {
            get => new(visualSize_.Width, TrackHeight);
            set
            {
                visualSize_.Width = Math.Max(visualSize_.Width, value.Width);
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
                upState_ = value;
                OnPropertyChanged(nameof(VisualUpSpriteSource));
            }
        }

        [Browsable(false)]
        public int DownState
        {
            set
            {
                downState_ = value;
                OnPropertyChanged(nameof(VisualDownSpriteSource));
            }
        }

        [Browsable(false)]
        public int ThumbState
        {
            set
            {
                thumbState_ = value;
                OnPropertyChanged(nameof(VisualThumbSpriteSource));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void RestoreSize()
        {
            widthInitialized_ = false;
            heightInitialized_ = false;

            UpdateVisualWidth();
            UpdateVisualHeight();

            OnPropertyChanged(nameof(VisualUpHeight));
            OnPropertyChanged(nameof(VisualThumbHeight));
            OnPropertyChanged(nameof(VisualDownHeight));
            OnPropertyChanged(nameof(TrackHeight));
            OnPropertyChanged(nameof(VisualSize));
            OnPropertyChanged(nameof(VisualRect));
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool IsVisualUpRectContainPoint(IntPoint _p)
        {
            SGUISpriteInfo visualUp = VisualUpSprite;
            if (visualUp.IsNull) return false;
            Rect rc = visualUp.Rect;
            rc.Width = visualSize_.Width;
            return rc.Contains(_p);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool IsVisualThumbRectContainPoint(IntPoint _p)
        {
            SGUISpriteInfo visualThumb = VisualThumbSprite;
            if (visualThumb.IsNull) return false;

            Rect visualThumbRect = visualThumb.Rect;
            visualThumbRect.Width = visualSize_.Width;

            if (!VisualUpSprite.IsNull)
                visualThumbRect.Y += VisualUpSprite.Height;

            return visualThumbRect.Contains(_p);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool IsVisualDownRectContainPoint(IntPoint _p)
        {
            SGUISpriteInfo visualDown = VisualDownSprite;
            if (visualDown.IsNull) return false;

            Rect visualDownRect = visualDown.Rect;
            visualDownRect.Width = visualSize_.Width;

            if (!VisualUpSprite.IsNull)
                visualDownRect.Y += VisualUpSprite.Height;

            if (!sprites_[IndexTrack].IsNull)
                visualDownRect.Y += TrackHeight;

            return visualDownRect.Contains(_p);
        }

        // 300, 400에 엘리먼트가 위치해있을때
        // 300, 400는 엘리먼트기준 0, 0의 좌표이다.
        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint ConvertElementPosition(IntPoint _p)
            => new(_p.X - visualPosition_.X, _p.Y - visualPosition_.Y);

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseMove(IntPoint _p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool elementContained = ContainPoint(_p);

            if (!elementContained)
            {
                State = StateNormal;
                UpState = IndexUpNormal;
                DownState = IndexDownNormal;
                ThumbState = IndexThumbNormal;
                return true;
            }

            State = StateOver;

            return false;
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

            _p = ConvertElementPosition(_p);

            UpState = IsVisualUpRectContainPoint(_p) ? IndexUpPressed : IndexUpNormal;
            DownState = IsVisualDownRectContainPoint(_p) ? IndexDownPressed : IndexDownNormal;
            ThumbState = IsVisualThumbRectContainPoint(_p) ? IndexThumbPressed : IndexThumbNormal;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseUp(IntPoint _p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(_p);
            State = StateNormal;
            UpState = IndexUpNormal;
            DownState = IndexDownNormal;
            ThumbState = IndexThumbNormal;

            if (!contained)
                return true;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIScrollBar scrollbar = new SGUIScrollBar();
            scrollbar.CopyFrom(this);
            scrollbar.visualSize_ = VisualSize;
            Array.Copy(sprites_, scrollbar.sprites_, sprites_.Length);
            return scrollbar;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            SGUISpriteInfoExt.TryGetSgaImgFileName(in sprites_, out string sga, out string img);
            root[JsonSgaKey] = sga;
            root[JsonImgKey] = img;
            root[JsonSpriteKey] = sprites_.ToFullString();
            root[JsonTrackSizeKey] = TrackSize.ToFullString();
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseJObject(JObject _root)
        {
            base.ParseJObject(_root);

            string sgaName = (string)_root[JsonSgaKey]!;

            if (sgaName == string.Empty)
                return;

            string imgName = (string)_root[JsonImgKey]!;
            string trackSizeString = (string)_root[JsonTrackSizeKey]!;

            IntSize trackSize = SizeEx.ParseFullString(trackSizeString);

            SgaImage img = ViewModel.PackManager.GetImg(sgaName, imgName);
            SgaPackage sga = img.Parent;
            int[] sprites = new int[TextureCount];
            StringEx.ParseIntNumberN((string)_root[JsonSpriteKey]!, sprites);
            SGUISpriteInfoExt.ParseInfo(sga, img, in sprites, in sprites_);

            int height = 0;

            if (!VisualUpSprite.IsNull)
                height += VisualUpSprite.Height;

            height += trackSize.Height;

            if (!VisualDownSprite.IsNull)
                height += VisualDownSprite.Height;

            visualSize_.Width = trackSize.Width;
            visualSize_.Height = Math.Max(height, MaxHeight);
        }
    }
}
