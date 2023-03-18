/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 7:04:01 AM
 *
 */

using SGToolsUI.ViewModel;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata.Ecma335;
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
using Xceed.Wpf.Toolkit.PropertyGrid;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using System.Reflection.PortableExecutable;
using Point = System.Windows.Point;
using Newtonsoft.Json.Linq;
using System.Windows.Threading;
using Xceed.Wpf.AvalonDock.Controls;
using Xceed.Wpf.Toolkit.PropertyGrid.Editors;
using PropertyItem = Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem;
using System.Drawing.Imaging;
using SGToolsCommon.Model;
using SGToolsUI.ModelTemplate.Main;
using SGToolsUI.Model.Main;

namespace SGToolsUI.CustomControl
{


    public class UIElementPropertyGrid : PropertyGrid, IDataDragReceiver, IKeyboardInputReceiver
    {
        public MainViewModel ViewModel { get; private set; }
        public ScrollViewer ScrollViewer { get; private set; }
        public ContextMenu ContextMenu { get; private set; }
        public MenuItem ClearMenuItem { get; private set; }
        public PropertyItemsControl PropertyItemsControl { get; private set; }
        private const BindingFlags PropertyFlag = BindingFlags.GetProperty | BindingFlags.SetProperty | BindingFlags.Public | BindingFlags.Instance;
        private string _selectProperty;

        public UIElementPropertyGrid()
        {
            Loaded += OnLoaded;
            SelectedPropertyItemChanged += OnSelectedPropertyItemChanged;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializeDescriptor();
            InitializeContextMenu();
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

        private void InitializeContextMenu()
        {
            ClearMenuItem = new MenuItemEx();
            ClearMenuItem.Header = "스프라이트 초기화";
            ClearMenuItem.Click += ClearMenuItemOnClick;

            ContextMenu = new ContextMenu();
            ContextMenu.Opened += ContextMenuOnOpened;
            ContextMenu.Items.Add(ClearMenuItem);

            AdvancedOptionsMenu = ContextMenu;
        }

        private void ContextMenuOnOpened(object sender, RoutedEventArgs e)
        {
            PropertyItem item = SelectedPropertyItem as PropertyItem;
            if (item == null) return;
            ClearMenuItem.IsEnabled = item.PropertyType == typeof(SGUISpriteInfo);
        }


        private void OnSelectedPropertyItemChanged(object sender, RoutedPropertyChangedEventArgs<PropertyItemBase> e)
        {
            PropertyItem selectedProperty = e.NewValue as PropertyItem;
            ViewModel.FocusedKeyboardInputReceiver = this;

            if (selectedProperty == null)
                return;

            // 프로퍼티 값이 들어있는 셀을 클릭하면 텍스트박스 전체 선택하도록
            if (!selectedProperty.IsReadOnly && selectedProperty.Editor is TextBox tb)
            {
                tb.Focus();
                tb.SelectAll();
                return;
            }

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

        public void DragEnd(Point p, object data)
        {

            /* 프로퍼티 그리드 내부 부모 관계도 (위에서부터 시작)
             * ContentPresenter
             * ContentControl: 없음
             * Border
             * Grid
             * Border
             * Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem
             * VirtualizingStackPanel
             * ItemsPresenter
             * ContentPresenter
             * Border
             * Border
             * Grid
             * Expander Header:버튼 Content:
             * Border
             * GroupItem: 버튼
             * VirtualizingStackPanel
             * ItemsPresenter
             * ScrollContentPresenter
             * Grid
             * ScrollViewer
             * Border
             * Xceed.Wpf.Toolkit.PropertyGrid.PropertyItemsControl Items.Count: 12
             * Grid
             * Grid
             * Border
             * SGToolsUI.CustomControl.UIElementPropertyGrid
             * Grid
             * Canvas
             * Grid
             * DockPanel
             * ContentPresenter
             * Grid
             * Border
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

        public bool TryGetSpriteInfoProperty(PropertyItem propertyItem, out PropertyInfo propInfo, out SGUIElement element, out SGUISpriteInfo spriteInfo)
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

        public bool TrySetSpriteInfoProperty(PropertyItem propertyItem, SGUISpriteInfo spriteInfo)
        {
            SGUIElement element = propertyItem.Instance as SGUIElement;
            if (element == null) return false;

            PropertyInfo propInfo = element.GetType().GetProperty(propertyItem.PropertyName, PropertyFlag);
            if (propInfo == null) return false;
            propInfo.SetValue(element, spriteInfo);
            return true;
        }

      
        private void ClearMenuItemOnClick(object sender, RoutedEventArgs e)
        {
            PropertyItem item = ClearMenuItem.DataContext as PropertyItem;
            if (item == null) return;
            if (item.PropertyType != typeof(SGUISpriteInfo)) return;
            TrySetSpriteInfoProperty(item, SGUISpriteInfo.Empty);
        }


        public void OnKeyDown(SGKey key)
        {
            SGUIElement element = SelectedObject as SGUIElement;

            if (element == null)
                return;

            switch (element.UIElementType)
            {
                case SGUIElementType.Group:         break;
                case SGUIElementType.Button:        break;
                case SGUIElementType.Label:         break;
                case SGUIElementType.Sprite:        break;
                case SGUIElementType.EditBox:       break;
                case SGUIElementType.CheckBox:      break;
                case SGUIElementType.ToggleButton:  break;
                case SGUIElementType.ScrollBar:     break;
                case SGUIElementType.ProgressBar:   break;
                default: throw new ArgumentOutOfRangeException();
            }
        }

        public void OnKeyUp(SGKey key)
        {
        }

        public bool ContainPoint(Point p) => SGToolsCommon.Extension.VisualEx.ContainPoint(this, p);

        public void SelectPropertyValue(bool newObject, ref string propertyName)
        {
            for (int i = 0; i < PropertyItemsControl.Items.Count; ++i)
            {
                var propertyItem = PropertyItemsControl.Items[i] as PropertyItem;
                if (propertyItem == null) continue;
                if (propertyItem.PropertyName != propertyName) continue;

                // 새롭게 할당된 오브젝트라면
                // 아직 하위 비주얼 UI 컨트롤들이 로딩되지 않은 상태라서 포커스를 잡을 수 없다.
                // 로딩되면 포커스를 잡아주도록 하자.
                // propertyItem.Editor.Focus();를 직접적으로 수행할 수가 없음
                if (newObject)
                    propertyItem.Loaded += (sender, args) => SelectValue(propertyItem);
                else
                    SelectValue(propertyItem);
                break;
            }

            void SelectValue(PropertyItem item)
            {
                item.Editor.Focus();
            }
        }

        public void SelectWithPropertyFocus(SGUIElement selectedElement, string propertyName)
        {
            // 이미 할당된 경우
            if (SelectedObject == selectedElement)
            {
                SelectPropertyValue(false, ref propertyName);
                return;
            }

            // 오브젝트 변경전 프로퍼티 아이템 수 : 카운트 0 (제일 초기)
            SelectedObject = selectedElement;
            // 오브젝트 변경후 프로퍼티 아이템 수 : 카운트 多 프로퍼티 아이템 세팅됨.
            SelectPropertyValue(true, ref propertyName);
        }

        private void OnSelectedObjectChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            string defaultSelectedPropertyName = _selectProperty.Length == 0 ? SGUIElement.VisualNameKey : _selectProperty;

            

            _selectProperty = string.Empty;
        }
    }

 
}
