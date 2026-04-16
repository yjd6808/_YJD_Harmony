/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 2:05:52 PM
 *
 */

using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;

using SGToolsCommon.Extension;

using Point = System.Windows.Point;

namespace SGToolsCommon.Model
{
    public enum DragState
    {
        None,
        Wait,           // 드래그 활성화 조건검사중 (이때부터 데이터 소지)
        Dragging,       // 드래그 활성화 상태
    }

    public class DataDragState : Bindable
    {
        private Point startPosition_;
        private DragState state_ = DragState.None;
        private UIElement? beginTarget_;

        public object? Data { get; set; }
        public List<IDataDragReceiver> EndTargets { get; } = new();

        public Point DragStartPosition
        {
            get => startPosition_;
            set
            {
                startPosition_ = value;
                OnPropertyChanged();
            }
        }

        public DragState State
        {
            get => state_;
            set
            {
                state_ = value;
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public DataDragState()
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnDragBegin(UIElement _beginTarget, Point _p, object _data)
        {
            beginTarget_ = _beginTarget;
            DragStartPosition = _p;
            State = DragState.Wait;
            Data = _data;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnDragMove(Point _p)
        {
            if (state_ == DragState.Wait)
            {
                double distance = _p.Distance(startPosition_);
                if (distance < Constant.DragActivateDistance)
                    return;

                state_ = DragState.Dragging;
                Mouse.OverrideCursor = SGToolsCommon.Resource.R.DragAndDropCursor.Value;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnDragEnd(Point _p)
        {
            if (State == DragState.Dragging)
                RaiseDragEndEvents(_p);

            Clear();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Clear()
        {
            beginTarget_ = null;
            Mouse.OverrideCursor = Cursors.Arrow;
            State = DragState.None;
            Data = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool RaiseDragEndEvents(Point _p)
        {
            for (int i = 0; i < EndTargets.Count; ++i)
            {
                if (EndTargets[i].ContainPoint(_p))
                {
                    EndTargets[i].DragEnd(_p, Data!);
                    return true;
                }
            }

            return false;
        }
    }
}
