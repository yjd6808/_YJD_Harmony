// jdyun 25/03/05(수)
using System;
using System.Windows;
using System.Windows.Media.Animation;

namespace SGToolsCommon.Extension
{
    public static class AnimationManager
    {
        // 저장효과를 위한 애니메이션
        public static Storyboard BeginFadeoutAnimation(FrameworkElement _element, double _timeSeconds)
        {
            DoubleAnimation myDoubleAnimation = new DoubleAnimation();
            myDoubleAnimation.From = 0;
            myDoubleAnimation.To = 100;
            myDoubleAnimation.Duration = new Duration(TimeSpan.FromSeconds(_timeSeconds));

            Storyboard.SetTargetName(myDoubleAnimation, _element.Name);
            Storyboard.SetTargetProperty(myDoubleAnimation, new PropertyPath(UIElement.OpacityProperty));

            Storyboard myWidthAnimatedButtonStoryboard = new Storyboard();
            myWidthAnimatedButtonStoryboard.Children.Add(myDoubleAnimation);
            myWidthAnimatedButtonStoryboard.Begin(_element);
            return myWidthAnimatedButtonStoryboard;
        }
    }
}
