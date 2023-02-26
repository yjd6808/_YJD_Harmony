using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Linq;

namespace SGToolsUI
{
    public class CustomCanvas : Canvas
    {
        private Rectangle? _selectionRect;
        private UIElement? _selectedElement;

        
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            MouseDown += CustomCanvas_MouseDown;

            _selectionRect = new Rectangle();
            _selectionRect.StrokeThickness = 2;
            _selectionRect.Stroke = Brushes.Green;
            _selectionRect.Visibility = Visibility.Hidden;
            Children.Add(_selectionRect);
        }

        private void HideSelectionRect()
        {
            _selectionRect.Visibility = Visibility.Hidden;
        }

        private void ShowSelectionRect()
        {
            _selectionRect.Visibility = Visibility.Visible;

            Children.Remove(_selectionRect);
            Children.Add(_selectionRect);
        }

        private void FitSelectionRect(Rect r)
        {
            _selectionRect.Width = r.Width;
            _selectionRect.Height = r.Height;

            SetLeft(_selectionRect, r.X);
            SetTop(_selectionRect, r.Y);
        }

        private void CustomCanvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point pos = e.GetPosition(this);
            bool find = false;

            foreach (UIElement g in Children)
            {
                if (g == _selectionRect)
                    continue;

                double x = GetLeft(g);
                double y = GetTop(g);

                var k1 = g.DesiredSize;

                Rect r = new Rect();
                r.X = x;
                r.Y = y;
                r.Width = k1.Width;
                r.Height = k1.Height;


                
                if (r.Contains(pos))
                {
                    if (_selectedElement == g)
                    {
                        _selectedElement = null;
                        HideSelectionRect();
                        continue;
                    }

                    _selectedElement = g;
                    FitSelectionRect(r);
                    ShowSelectionRect();
                    find = true;
                    break;
                }
            }

            if (find == false)
            {
                _selectedElement = null;
                HideSelectionRect();
            }
        }

        protected override Size MeasureOverride(Size constraint)
        {
            base.MeasureOverride(constraint);

            var size = new Size();

            foreach (var child in Children.OfType<FrameworkElement>())
            {
                var x = GetLeft(child) + child.Width;
                var y = GetTop(child) + child.Height;

                if (!double.IsNaN(x) && size.Width < x)
                {
                    size.Width = x;
                }

                if (!double.IsNaN(y) && size.Height < y)
                {
                    size.Height = y;
                }
            }

            return size;
        }
    }
}
