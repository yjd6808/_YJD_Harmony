/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 2:05:52 PM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsCommon.Extension;
using SGToolsUI.CustomControl;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model
{
    public enum DragState
    {
        None,
        Wait,
        Dragging,
    }

    public class DataDragState : Bindable
    {
        public DataDragState(MainViewModel viewModel)
        {
            _viewModel = viewModel;
        }

        public object Data { get; set; }

        public Point DragStartPosition
        {
            get => _startPosition;
            set
            {
                _startPosition = value;
                OnPropertyChanged();
            }
        }

        public DragState State
        {
            get => _state;
            set
            {
                _state = value;
                OnPropertyChanged();
            }
        }

        public void OnDragMove(Point p)
        {
            if (_state == DragState.Wait)
            {
                double distance = p.Distance(_startPosition);
                if (distance < Constant.DragActivateDistance)
                    return;

                _state = DragState.Dragging;
                Mouse.OverrideCursor = SGToolsCommon.Resource.R.DragAndDropCursor.Value;
                
            }
        }

        public void OnDragEnd(Point p)
        {



            Mouse.OverrideCursor = Cursors.Arrow;
            State = DragState.None;
            Data = null;
        }

        private Point _startPosition;
        private DragState _state = DragState.None;
        private MainViewModel _viewModel;
    }


}
