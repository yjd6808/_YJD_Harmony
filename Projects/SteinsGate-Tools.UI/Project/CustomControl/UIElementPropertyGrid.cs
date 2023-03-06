/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 7:04:01 AM
 *
 */

using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
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
using SGToolsCommon.Extension;
using SGToolsUI.Model;
using Xceed.Wpf.Toolkit.PropertyGrid;

namespace SGToolsUI.CustomControl
{
    public class UIElementPropertyGrid : PropertyGrid
    {
        public MainViewModel ViewModel { get; private set; }
        public ScrollViewer ScrollViewer { get; private set; }

        public UIElementPropertyGrid()
        {
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializeDefinitions();
        }

        private void InitializeViewModel()
        {
            ViewModel = DataContext as MainViewModel;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("UIElementTreeView에서 뷰모델 초기화 실패");
            ScrollViewer = ViewModel.View.UIElementTreeViewScrollViewer;
        }

        private void InitializeDefinitions()
        {
            PropertyDefinition CreateDefinition(string name, int order)
            {
                return new PropertyDefinition()
                {
                    DisplayName = name,
                    DisplayOrder = order
                };
            }

            typeof(SGUIElement).PrintAllPropertiesBrowsable(BindingFlags.Default | BindingFlags.Public | BindingFlags.GetProperty | BindingFlags.Instance);


            //PropertyDefinitions.Add(CreateDefinition("VisualName", 1));

            //PropertyDefinition nameProperty = new PropertyDefinition();
            //nameProperty.TargetProperties.Add("DefineName");
            //nameProperty.Order = 1;
        }
    }
}
