/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 7:04:01 AM
 *
 */

using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
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
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.Model;
using Xceed.Wpf.Toolkit.PropertyGrid;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using System.Reflection.PortableExecutable;
using Point = System.Windows.Point;
using Newtonsoft.Json.Linq;

namespace SGToolsUI.CustomControl
{
    public class UIElementPropertyGrid : PropertyGrid, IDataDragReceiver
    {
        public MainViewModel ViewModel { get; private set; }
        public ScrollViewer ScrollViewer { get; private set; }
        public PropertyItemsControl PropertyItemsControl { get; private set; }
        private const BindingFlags PropertyFlag = BindingFlags.GetProperty | BindingFlags.Public | BindingFlags.Instance;

        public UIElementPropertyGrid()
        {
            Loaded += OnLoaded;
            SelectedPropertyItemChanged += OnSelectedPropertyItemChanged;

        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializeDescriptor();
        }

        private void InitializeDescriptor()
        {
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
            PropertyItemsControl = this.FindChild<PropertyItemsControl>();

            
        }

        private void OnSelectedPropertyItemChanged(object sender, RoutedPropertyChangedEventArgs<PropertyItemBase> e)
        {
            PropertyItem selectedProperty = e.NewValue as PropertyItem;

            if (selectedProperty == null)
                return;

            if (!TryGetSpriteInfoProperty(selectedProperty, out PropertyInfo propInfo, out SGUIElement element, out SGUISpriteInfo spriteInfo))
                return;

            if (spriteInfo.IsNull)
                return;

            ViewModel.Commander.SelectSgaPackage.Execute(spriteInfo.Sga);
            ViewModel.Commander.SelectSgaElement.Execute(spriteInfo.Img.Header);
            ViewModel.Commander.SelectSgaSprite.Execute(spriteInfo.Sprite);

            ViewModel.View.PackageListBox.ScrollIntoView(spriteInfo.Sga);
            ViewModel.View.ElementListBox.ScrollIntoView(spriteInfo.Img.Header);
            ViewModel.View.SpriteListBox.ScrollIntoView(spriteInfo.Sprite);
        }

        public void DragEnd(object data)
        {

            /* 프로퍼티 그리드 내부 부모 관계도 (위에서부터 시작)
             * System.Windows.Controls.ContentPresenter
             * System.Windows.Controls.ContentControl: 없음
             * System.Windows.Controls.Border
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Border
             * Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem
             * System.Windows.Controls.VirtualizingStackPanel
             * System.Windows.Controls.ItemsPresenter
             * System.Windows.Controls.ContentPresenter
             * System.Windows.Controls.Border
             * System.Windows.Controls.Border
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Expander Header:버튼 Content:
             * System.Windows.Controls.Border
             * System.Windows.Controls.GroupItem: 버튼
             * System.Windows.Controls.VirtualizingStackPanel
             * System.Windows.Controls.ItemsPresenter
             * System.Windows.Controls.ScrollContentPresenter
             * System.Windows.Controls.Grid
             * System.Windows.Controls.ScrollViewer
             * System.Windows.Controls.Border
             * Xceed.Wpf.Toolkit.PropertyGrid.PropertyItemsControl Items.Count: 12
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Border
             * SGToolsUI.CustomControl.UIElementPropertyGrid
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Canvas
             * System.Windows.Controls.Grid
             * System.Windows.Controls.DockPanel
             * System.Windows.Controls.ContentPresenter
             * System.Windows.Controls.Grid
             * System.Windows.Controls.Border
             * SGToolsUI.View.MainView
             */


            SgaSprite sprite = data as SgaSprite;

            if (sprite == null)
                return;

            Point pos = Mouse.GetPosition(this);
            var hit = this.HitTest<UIElementPropertyGrid, PropertyItem>(pos);

            if (hit == null)
                return;

            PropertyItem propertyItem = hit.Item;
            if (!TryGetSpriteInfoProperty(propertyItem, out PropertyInfo propInfo, out SGUIElement element, out SGUISpriteInfo spriteInfo))
                return;

            bool apply = false;

            if (spriteInfo.IsNull)
            {
                apply = MessageBoxEx.ShowTopMost(
                    $"[{propertyItem.DisplayName}]에 스프라이트 데이터를 넣어으시겠습니까?",
                    "확인",
                    MessageBoxButton.YesNo,
                    MessageBoxImage.Question) == MessageBoxResult.Yes;
            }
            else
            {
                apply = MessageBoxEx.ShowTopMost(
                    $"{propertyItem.DisplayName}에 이미 스프라이트 정보가 할당되어있습니다.\n" +
                    $"스프라이트 데이터를 넣어으시겠습니까?\n\n" +
                    $"{spriteInfo.Question}",
                    "확인",
                    MessageBoxButton.YesNo,
                    MessageBoxImage.Question) == MessageBoxResult.Yes;
            }

            if (apply)
                propInfo.SetValue(element, new SGUISpriteInfo(sprite));
        }

        private bool TryGetSpriteInfoProperty(PropertyItem propertyItem, out PropertyInfo propInfo, out SGUIElement element, out SGUISpriteInfo spriteInfo)
        {
            spriteInfo = new SGUISpriteInfo();
            element = propertyItem.Instance as SGUIElement;
            propInfo = null;
            string propName = propertyItem.PropertyName;

            if (element == null)
                return false;

            propInfo = element.GetType().GetProperty(propName, PropertyFlag);

            if (propInfo == null)
                return false;

            if (propInfo.PropertyType != typeof(SGUISpriteInfo))
                return false;

            spriteInfo = (SGUISpriteInfo)propInfo.GetValue(element);
            return true;
        }


        public void OnKeyDown(SGKey key)
        {
            SGUIElement element = SelectedObject as SGUIElement;

            if (element == null)
                return;

            switch (element.UIElementType)
            {
                case SGUIElementType.Group:
                    break;
                case SGUIElementType.Button:
                    break;
                case SGUIElementType.Label:
                    break;
                case SGUIElementType.Sprite:
                    break;
                case SGUIElementType.EditBox:
                    break;
                case SGUIElementType.CheckBox:
                    break;
                case SGUIElementType.ToggleButton:
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public bool ContainPoint(Point p) => SGToolsCommon.Extension.VisualEx.ContainPoint(this, p);
    }

 
}
