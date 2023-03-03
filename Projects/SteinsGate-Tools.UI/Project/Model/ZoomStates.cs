/*
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
        public ZoomState()
        {
        }

        public double ZoomLevelX
        {
            get => _zoomLevelX;
            set
            {
                _zoomLevelX = value;
                OnPropertyChanged("BaseWindowWidth");
                OnPropertyChanged();
                OnPropertyChanged("ZoomLevelXString");
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

        public string ZoomLevelXString => $"{_zoomLevelX * 100.0:F0}" + '%';
        public string ZoomLevelYString => $"{_zoomLevelY * 100.0:F0}" + '%';



        public double BaseCanvasStatusBarWidth => _baseCanvasStatusBarWidth * _zoomLevelX;

        // 윈도우 로딩 끝나고 크기 설정이 가능하기때문에 어쩔수없이 set 추가함.
        public double BaseWindowWidth
        {
            get => _baseWindowWidth * _zoomLevelX;
            set => _baseWindowWidth = value;
        }

        public double BaseWindowHeight
        {
            get => _baseWindowHeight * ZoomLevelY;
            set => _baseWindowHeight = value;
        }

        private double _baseCanvasStatusBarWidth = Constant.CanvasWidth;
        private double _baseWindowWidth;
        private double _baseWindowHeight;
        private double _zoomLevelX = 1.0;
        private double _zoomLevelY = 1.0;
    }
}
