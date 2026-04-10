/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:37:01 PM
 *
 */

using System;
using SGToolsCommon;

namespace SGToolsUI.Model.Main
{
    public class ZoomState : Bindable
    {
        private double baseCanvasStatusBarWidth_ = Constant.CanvasWidth;
        private double basePropertyGridHeight_ = Constant.PropertyGridHeight;
        private double baseWindowWidth_ = -1;
        private double baseWindowHeight_ = -1;
        private double zoomLevelX_ = 1.0;
        private double zoomLevelY_ = 1.0;

        //////////////////////////////////////////////////////////////////////////////////
        public double ZoomLevelX
        {
            get => zoomLevelX_;
            set
            {
                zoomLevelX_ = value;

                OnPropertyChanged();
                OnPropertyChanged("IsNotDefaultZoomLevel");
                OnPropertyChanged("ZoomLevelXString");
                OnPropertyChanged("ZoomLevelXIntegerPercent");
                OnPropertyChanged("BaseWindowWidth");
                OnPropertyChanged("BaseCanvasStatusBarWidth");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public double ZoomLevelY
        {
            get => zoomLevelY_;
            set
            {
                zoomLevelY_ = value;

                OnPropertyChanged();
                OnPropertyChanged("BaseWindowHeight");
                OnPropertyChanged("BasePropertyGridHeight");
            }
        }

        // 기준에서 얼마나 변경되는지
        public double ZoomWidthDelta => Constant.CanvasWidth * zoomLevelX_ - Constant.CanvasWidth;
        public double ZoomHeightDelta => Constant.CanvasHeight * zoomLevelY_ - Constant.CanvasHeight;
        public string ZoomLevelXString => $"{zoomLevelX_ * 100.0:F0}" + '%';
        public string ZoomLevelYString => $"{zoomLevelY_ * 100.0:F0}" + '%';
        public bool IsNotDefaultZoomLevel => (int)(zoomLevelX_ * 100) != 100;    // 줌이 가장 기본 상태 100%가 아닌 경우

        public double BaseCanvasStatusBarWidth => baseCanvasStatusBarWidth_ + ZoomWidthDelta;
        public double BasePropertyGridHeight => basePropertyGridHeight_ + ZoomHeightDelta;

        //////////////////////////////////////////////////////////////////////////////////
        // 윈도우 로딩 끝나고 크기 설정이 가능하기때문에 어쩔수없이 set 추가함, 2번 설정 방지를 위해 익셉션
        public double BaseWindowWidth
        {
            get => baseWindowWidth_ + ZoomWidthDelta;
            set
            {
                if (baseWindowWidth_ >= 0)
                    throw new Exception("이미 너비가 설정되었습니다.");

                baseWindowWidth_ = value;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public double BaseWindowHeight
        {
            get => baseWindowHeight_ + ZoomHeightDelta;
            set
            {
                if (baseWindowHeight_ >= 0)
                    throw new Exception("이미 너비가 설정되었습니다.");

                baseWindowHeight_ = value;
            }
        }
    }
}
