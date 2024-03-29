﻿/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 2:05:52 PM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsCommon;
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
        public DataDragState()
        {
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

        public List<IDataDragReceiver> EndTargets { get; } = new();

        public void OnDragBegin(UIElement beginTarget, Point p, object data)
        {
            _beginTarget = beginTarget;
            DragStartPosition = p;
            State = DragState.Wait;
            Data = data;
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
            if (State == DragState.Dragging)
                RaiseDragEndEvents(p);

            Clear();
        }

        private bool RaiseDragEndEvents(Point p)
        {
            for (int i = 0; i < EndTargets.Count; ++i)
            {
                if (EndTargets[i].ContainPoint(p))
                {
                    EndTargets[i].DragEnd(p, Data);
                    return true;
                }
            }

            return false;
        }

        private Point _startPosition;
        private DragState _state = DragState.None;
        private UIElement _beginTarget;

        public void Clear()
        {
            _beginTarget = null;
            Mouse.OverrideCursor = Cursors.Arrow;
            State = DragState.None;
            Data = null;
        }
    }


}
