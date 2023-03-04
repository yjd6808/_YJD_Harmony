﻿/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:37:01 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using SGToolsCommon;

namespace SGToolsUI.Model
{
    public class ZoomState : Bindable
    {
        public double ZoomLevelX
        {
            get => _zoomLevelX;
            set
            {
                _zoomLevelX = value;
                
                OnPropertyChanged();
                OnPropertyChanged("IsNotDefaultZoomLevel");
                OnPropertyChanged("ZoomLevelXString");
                OnPropertyChanged("ZoomLevelXIntegerPercent");
                OnPropertyChanged("BaseWindowWidth");
                OnPropertyChanged("BaseCanvasStatusBarWidth");
            }
        }

        public double ZoomLevelY
        {
            get => _zoomLevelY;
            set
            {
                _zoomLevelY = value;
                OnPropertyChanged("BaseWindowHeight");
                OnPropertyChanged();
            }
        }

        // 기준에서 얼마나 변경되는지
        public double ZoomWidthDelta => Constant.CanvasWidth * _zoomLevelX - Constant.CanvasWidth;
        public double ZoomHeightDelta => Constant.CanvasHeight * _zoomLevelY - Constant.CanvasHeight;
        public string ZoomLevelXString => $"{_zoomLevelX * 100.0:F0}" + '%';
        public string ZoomLevelYString => $"{_zoomLevelY * 100.0:F0}" + '%';
        public bool IsNotDefaultZoomLevel => (int)(_zoomLevelX * 100) != 100;    // 줌이 가장 기본 상태 100%가 아닌 경우

        public double BaseCanvasStatusBarWidth => _baseCanvasStatusBarWidth + ZoomWidthDelta;

        // 윈도우 로딩 끝나고 크기 설정이 가능하기때문에 어쩔수없이 set 추가함.
        public double BaseWindowWidth
        {
            get => _baseWindowWidth + ZoomWidthDelta;
            set => _baseWindowWidth = value;
        }

        public double BaseWindowHeight
        {
            get => _baseWindowHeight + ZoomHeightDelta;
            set => _baseWindowHeight = value;
        }

        private double _baseCanvasStatusBarWidth = Constant.CanvasWidth;
        private double _baseWindowWidth;
        private double _baseWindowHeight;
        private double _zoomLevelX = 1.0;
        private double _zoomLevelY = 1.0;

    }
}
