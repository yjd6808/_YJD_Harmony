﻿/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 12:28:01 AM
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

namespace SGToolsCommon.Model
{
    public interface IDataDragReceiver
    {
        void DragEnd(Point p, object data);
        bool ContainPoint(Point p);
    }
}
