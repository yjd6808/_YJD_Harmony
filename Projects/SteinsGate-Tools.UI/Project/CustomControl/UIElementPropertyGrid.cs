/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 7:04:01 AM
 *
 */

using System;
using System.ComponentModel;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;
using SGToolsUI.ModelTemplate.Main;
using SGToolsUI.ViewModel;
using Xceed.Wpf.Toolkit.PropertyGrid;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using PropertyItem = Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem;

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
        private string selectProperty_;

        //////////////////////////////////////////////////////////////////////////////////
        public UIElementPropertyGrid()
        {
            Loaded += OnLoaded;
            SelectedPropertyItemChanged += OnSelectedPropertyItemChanged;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            InitializeViewModel();
            InitializeDescriptor();
            InitializeContextMenu();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeDescriptor()
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
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

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeContextMenu()
        {
            ClearMenuItem = new MenuItem();
            ClearMenuItem.Header = "스프라이트 초기화";
            ClearMenuItem.Click += ClearMenuItemOnClick;

            ContextMenu = new ContextMenu();
            ContextMenu.Opened += ContextMenuOnOpened;
            ContextMenu.Items.Add(ClearMenuItem);

            AdvancedOptionsMenu = ContextMenu;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ContextMenuOnOpened(object _sender, RoutedEventArgs _e)
        {
            PropertyItem item = SelectedPropertyItem as PropertyItem;
            if (item == null) return;
            ClearMenuItem.IsEnabled = item.PropertyType == typeof(SGUISpriteInfo);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnSelectedPropertyItemChanged(object _sender, RoutedPropertyChangedEventArgs<PropertyItemBase> _e)
        {
            PropertyItem selectedProperty = _e.NewValue as PropertyItem;
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

        //////////////////////////////////////////////////////////////////////////////////
        public void DragEnd(IntPoint _p, object _data)
        {
            SgaSprite sprite = _data as SgaSprite;

            if (sprite == null)
                return;

            IntPoint pos = Mouse.GetPosition(this);
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

        //////////////////////////////////////////////////////////////////////////////////
        public bool TryGetSpriteInfoProperty(PropertyItem _propertyItem, out PropertyInfo _propInfo, out SGUIElement _element, out SGUISpriteInfo _spriteInfo)
        {
            _spriteInfo = new SGUISpriteInfo();
            _element = _propertyItem.Instance as SGUIElement;
            _propInfo = null;
            string propName = _propertyItem.PropertyName;

            if (_element == null)
                return false;

            _propInfo = _element.GetType().GetProperty(propName, PropertyFlag);

            if (_propInfo == null)
                return false;

            if (_propInfo.PropertyType != typeof(SGUISpriteInfo))
                return false;

            _spriteInfo = (SGUISpriteInfo)_propInfo.GetValue(_element);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool TrySetSpriteInfoProperty(PropertyItem _propertyItem, SGUISpriteInfo _spriteInfo)
        {
            SGUIElement element = _propertyItem.Instance as SGUIElement;
            if (element == null) return false;

            PropertyInfo propInfo = element.GetType().GetProperty(_propertyItem.PropertyName, PropertyFlag);
            if (propInfo == null) return false;
            propInfo.SetValue(element, _spriteInfo);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ClearMenuItemOnClick(object _sender, RoutedEventArgs _e)
        {
            PropertyItem item = ClearMenuItem.DataContext as PropertyItem;
            if (item == null) return;
            if (item.PropertyType != typeof(SGUISpriteInfo)) return;
            TrySetSpriteInfoProperty(item, SGUISpriteInfo.Empty);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnKeyDown(SGKey _key)
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
                case SGUIElementType.Static:        break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnKeyUp(SGKey _key)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnLostFocus()
        {
            this.FocusClear();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool ContainPoint(IntPoint _p)
            => SGToolsCommon.Extension.VisualEx.ContainPoint(this, _p);

        //////////////////////////////////////////////////////////////////////////////////
        public void SelectPropertyValue(bool _newObject, ref string _propertyName)
        {
            for (int i = 0; i < PropertyItemsControl.Items.Count; ++i)
            {
                var propertyItem = PropertyItemsControl.Items[i] as PropertyItem;
                if (propertyItem == null) continue;
                if (propertyItem.PropertyName != _propertyName) continue;

                if (_newObject)
                    propertyItem.Loaded += (_sender, _args) => SelectValue(propertyItem);
                else
                    SelectValue(propertyItem);
                break;
            }

            void SelectValue(PropertyItem item)
            {
                item.Editor.Focus();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void SelectWithPropertyFocus(SGUIElement _selectedElement, string _propertyName)
        {
            // 이미 할당된 경우
            if (SelectedObject == _selectedElement)
            {
                SelectPropertyValue(false, ref _propertyName);
                return;
            }

            SelectedObject = _selectedElement;
            SelectPropertyValue(true, ref _propertyName);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnSelectedObjectChanged(object _sender, RoutedPropertyChangedEventArgs<object> _e)
        {
            string defaultSelectedPropertyName = selectProperty_.Length == 0 ? SGUIElement.VisualNameKey : selectProperty_;
            selectProperty_ = string.Empty;
        }
    }
}
