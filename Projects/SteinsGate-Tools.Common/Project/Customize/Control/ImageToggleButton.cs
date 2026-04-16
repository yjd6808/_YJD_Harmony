// jdyun
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Control
{
    public class ImageToggleButton : Button
    {
        public static readonly DependencyProperty ToggledProperty =
            DependencyProperty.Register(nameof(Toggled), typeof(bool), typeof(ImageToggleButton), new PropertyMetadata(false));

        public bool Toggled
        {
            get { return (bool)GetValue(ToggledProperty); }
            set
            {
                SetValue(ToggledProperty, value);
                UpdateCurrentImageSource();
            }
        }

        public static readonly DependencyProperty NormalImageSourceProperty =
            DependencyProperty.RegisterAttached(nameof(NormalImageSource), typeof(ImageSource), typeof(ImageToggleButton), new UIPropertyMetadata(null));

        public ImageSource NormalImageSource
        {
            get { return (ImageSource)GetValue(NormalImageSourceProperty); }
            set { SetValue(NormalImageSourceProperty, value); }
        }

        public static readonly DependencyProperty ToggledImageSourceProperty =
            DependencyProperty.RegisterAttached(nameof(ToggledImageSource), typeof(ImageSource), typeof(ImageToggleButton), new UIPropertyMetadata(null));

        public ImageSource ToggledImageSource
        {
            get { return (ImageSource)GetValue(ToggledImageSourceProperty); }
            set { SetValue(ToggledImageSourceProperty, value); }
        }

        private static readonly DependencyPropertyKey CurrentImageSourcePropertyKey
            = DependencyProperty.RegisterReadOnly(
                nameof(CurrentImageSource),
                typeof(ImageSource), typeof(ImageToggleButton),
                new FrameworkPropertyMetadata(null,
                    FrameworkPropertyMetadataOptions.None));

        public static readonly DependencyProperty s_CurrentImageSourceProperty
            = CurrentImageSourcePropertyKey.DependencyProperty;

        public ImageSource CurrentImageSource
        {
            get
            {
                return (ImageSource)GetValue(s_CurrentImageSourceProperty);
            }
            protected set
            {
                SetValue(CurrentImageSourcePropertyKey, value);
            }
        }

        private void UpdateCurrentImageSource()
        {
            if (!Toggled)
                CurrentImageSource = NormalImageSource;
            else
                CurrentImageSource = ToggledImageSource;
        }

        protected override void OnInitialized(EventArgs _e)
        {
            // CurrentImageSource를 처음에 업데이트 하기 위함
            // 로드시에 해주는 이유는 바인딩이 완료되는 최후의 시점으로 보장해주기 위함이다.
            // Initialize 시점에는 아직 바인딩이 완료되지 않은 시점일 수도 있음.
            Loaded += (_sender, _args) =>
            {
                UpdateCurrentImageSource();
            };

            base.OnInitialized(_e);
        }

        protected override void OnClick()
        {
            Toggled = !Toggled;
            UpdateCurrentImageSource();
            base.OnClick();
        }
    }
}
