// jdyun 24/11/10(일)

using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Control
{
    public class InternalDataGridItemTextChangedEventArgs : RoutedEventArgs
    {
        public InternalDataGridItem Item { get; }
        public InternalDataGridItemTextChangedEventArgs(RoutedEvent _routedEvent, InternalDataGridItem _item) : base(_routedEvent)
        {
            Item = _item;
        }
    }

    public class InternalDataGridComboBoxSelectionChangedEventArgs : RoutedEventArgs
    {
        public InternalDataGridItem Item { get; }
        public InternalDataGridComboBoxSelectionChangedEventArgs(RoutedEvent _routedEvent, InternalDataGridItem _item) : base(_routedEvent)
        {
            Item = _item;
        }
    }

    public delegate void IDGEventHandler(InternalDataGridItem _sender);
    public class InternalDataGridItem : Bindable
    {
        public bool IsFirstItem => Index == 0;
        public bool IsLastItem => Index == Container.Count() - 1;

        public object? ComboBoxData1 => ComboBox1?.SelectedItem;

        private int comboBoxIndex1_ = 0;
        public int ComboBoxIndex1
        {
            get => comboBoxIndex1_;
            set
            {
                if (comboBoxIndex1_ != value)
                {
                    comboBoxIndex1_ = value;
                    OnPropertyChanged();
                }
            }
        }

        private string text1_ = string.Empty;
        public string Text1
        {
            get => text1_;
            set
            {
                if (!text1_.Equals(value))
                {
                    text1_ = value;
                    OnPropertyChanged();
                }
            }
        }

        private string text2_ = string.Empty;
        public string Text2
        {
            get => text2_;
            set
            {
                if (!text2_.Equals(value))
                {
                    text2_ = value;
                    OnPropertyChanged();
                }
            }
        }

        public int Index { get; private set; }
        public string IndexName => $"{Index + 1}번째";
        public IEnumerable<InternalDataGridItem> Container { get; }
        public event IDGEventHandler? Loaded;
        public bool IsLoaded { get; set; }

        public ComboBox ComboBox1 { get; set; } = null!;
        public AutoCompleteTextBox TextBox1 { get; set; } = null!;
        public AutoCompleteTextBox TextBox2 { get; set; } = null!;
        public DataGridCell ComboBoxCell1 { get; set; } = null!;
        public DataGridCell TextBoxCell1{ get; set; } = null!;
        public DataGridCell TextBoxCell2 { get; set; } = null!;

        public InternalDataGridItem(IEnumerable<InternalDataGridItem> _container)
        {
            Container = _container;
        }

        public void Refresh()
        {
            int idx = 0;
            foreach (var item in Container)
            {
                if (item != this)
                {
                    ++idx;
                    continue;
                }

                Index = idx;
                OnPropertyChanged(nameof(IsFirstItem));
                OnPropertyChanged(nameof(IsLastItem));
                OnPropertyChanged(nameof(IndexName));
                break;
            }
        }

        public void SetEnable(int _column, bool _enable)
        {
            if (_column == 0)
            {
                if (ComboBoxCell1 != null) 
                    ComboBoxCell1.IsEnabled = _enable;
                if (ComboBox1 != null) 
                    ComboBox1.IsEnabled = _enable;
            }
            else if (_column == 1)
            {
                if (TextBoxCell1 != null)
                    TextBoxCell1.IsEnabled = _enable;
                if (TextBox1 != null) 
                    TextBox1.IsEnabled = _enable;
            }
            else if (_column == 2)
            {
                if (TextBoxCell2 != null)
                    TextBoxCell2.IsEnabled = _enable;
                if (TextBox2 != null)
                    TextBox2.IsEnabled = _enable;
            }
        }

        public void SetSubTextBoxPlaceholder(string _placeholder)
        {
            if (TextBox2 != null)
                TextBox2.Placeholder = _placeholder;
        }

        public void SetSelectedIndex(int _index)
        {
            if (ComboBox1 != null)
                ComboBox1.SelectedIndex = _index;
        }

        public void OnLoaded()
        {
            Loaded?.Invoke(this);
        }
    }

    public partial class TextBoxGeneratorDataGrid : UserControl
    {
        public ObservableCollection<InternalDataGridItem> Items { get; } = new();   // Frist는 무조건 두는걸 전제로 하기 땜에 외부에서 사용시 첫번째 아이템 제거안되도록 주의해야함.
        public InternalDataGridItem First { get; private set; }
        public ViewModel VM { get; }

        public TextBoxGeneratorDataGrid()
        {
            VM = new ViewModel(this);
            InitializeComponent();
            Items.Add(First = new InternalDataGridItem(Items));
        }

        public static readonly DependencyProperty MaxItemCountProperty =
            DependencyProperty.Register(nameof(MaxItemCount), typeof(int), typeof(TextBoxGeneratorDataGrid), new PropertyMetadata(5));

        public int MaxItemCount
        {
            get => (int)GetValue(MaxItemCountProperty);
            set => SetValue(MaxItemCountProperty, value);
        }

        public int Count => Items.Count;

        #region _UNUSED
        public static readonly DependencyProperty TextBoxTemplateProperty =
            DependencyProperty.Register(nameof(TextBoxTemplate), typeof(DataTemplate), typeof(TextBoxGeneratorDataGrid), new PropertyMetadata(null));

        public DataTemplate TextBoxTemplate
        {
            get => (DataTemplate)GetValue(TextBoxTemplateProperty);
            set => SetValue(TextBoxTemplateProperty, value);
        }

        public static readonly DependencyProperty AddButtonTemplateProperty =
            DependencyProperty.Register(nameof(AddButtonTemplate), typeof(DataTemplate), typeof(TextBoxGeneratorDataGrid), new PropertyMetadata(null));

        public DataTemplate AddButtonTemplate
        {
            get => (DataTemplate)GetValue(AddButtonTemplateProperty);
            set => SetValue(AddButtonTemplateProperty, value);
        }
        #endregion

        public static readonly DependencyProperty PlaceholderSuffix1Property =
            DependencyProperty.Register(nameof(PlaceholderSuffix1), typeof(string), typeof(TextBoxGeneratorDataGrid), new PropertyMetadata(string.Empty));

        public string PlaceholderSuffix1
        {
            get => (string)GetValue(PlaceholderSuffix1Property);
            set => SetValue(PlaceholderSuffix1Property, value);
        }

        public static readonly DependencyProperty PlaceholderSuffix2Property =
            DependencyProperty.Register(nameof(PlaceholderSuffix2), typeof(string), typeof(TextBoxGeneratorDataGrid), new PropertyMetadata(string.Empty));

        public string PlaceholderSuffix2
        {
            get => (string)GetValue(PlaceholderSuffix2Property);
            set => SetValue(PlaceholderSuffix2Property, value);
        }

        public static readonly DependencyProperty Candidates1Property =
            DependencyProperty.Register(nameof(Candidates1), typeof(List<string>), typeof(TextBoxGeneratorDataGrid),
                new PropertyMetadata(new List<string>()));

        public List<string> Candidates1
        {
            get => (List<string>)GetValue(Candidates1Property);
            set => SetValue(Candidates1Property, value);
        }

        public static readonly DependencyProperty Candidates2Property =
            DependencyProperty.Register(nameof(Candidates2), typeof(List<string>), typeof(TextBoxGeneratorDataGrid),
                new PropertyMetadata(new List<string>()));

        public List<string> Candidates2
        {
            get => (List<string>)GetValue(Candidates2Property);
            set => SetValue(Candidates2Property, value);
        }

        public static readonly DependencyProperty ComboBoxItems1Property =
            DependencyProperty.Register(nameof(ComboBoxItems1), typeof(List<object>), typeof(TextBoxGeneratorDataGrid),
                new PropertyMetadata(null));

        public List<object> ComboBoxItems1
        {
            get => (List<object>)GetValue(ComboBoxItems1Property);
            set => SetValue(ComboBoxItems1Property, value);
        }

        public static readonly DependencyProperty ComboBoxItems2Property =
            DependencyProperty.Register(nameof(ComboBoxItems2), typeof(List<object>), typeof(TextBoxGeneratorDataGrid),
                new PropertyMetadata(null));
        public List<object> ComboBoxItems2
        {
            get => (List<object>)GetValue(ComboBoxItems2Property);
            set => SetValue(ComboBoxItems2Property, value);
        }

        public DataGridLength W0
        {
            get => TemplateColumn0.Width;
            set => TemplateColumn0.Width = value;
        }

        public DataGridLength W1
        {
            get => TemplateColumn1.Width;
            set => TemplateColumn1.Width = value;
        }

        public DataGridLength W2
        {
            get => TemplateColumn2.Width;
            set => TemplateColumn2.Width = value;
        }

        public DataGridLength W3
        {
            get => TemplateColumn3.Width;
            set => TemplateColumn3.Width = value;
        }
        public DataGridLength W4
        {
            get => TemplateColumn4.Width;
            set => TemplateColumn4.Width = value;
        }

        public double RowHeight
        {
            get => ItemsDataGrid.RowHeight;
            set => ItemsDataGrid.RowHeight = value;
        }

        public static readonly RoutedEvent TextChangedEvent1 =
            EventManager.RegisterRoutedEvent(nameof(TextChanged1), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(TextBoxGeneratorDataGrid));

        public event RoutedEventHandler TextChanged1
        {
            add => AddHandler(TextChangedEvent1, value);
            remove => RemoveHandler(TextChangedEvent1, value);
        }

        public static readonly RoutedEvent TextChangedEvent2 =
            EventManager.RegisterRoutedEvent(nameof(TextChanged2), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(TextBoxGeneratorDataGrid));

        public event RoutedEventHandler TextChanged2
        {
            add => AddHandler(TextChangedEvent2, value);
            remove => RemoveHandler(TextChangedEvent2, value);
        }

        public static readonly RoutedEvent ComboBoxSelectionEvent1 =
            EventManager.RegisterRoutedEvent(nameof(ComboBoxSelection1), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(TextBoxGeneratorDataGrid));
        public event RoutedEventHandler ComboBoxSelection1
        {
            add => AddHandler(ComboBoxSelectionEvent1, value);
            remove => RemoveHandler(ComboBoxSelectionEvent1, value);
        }
        public static readonly RoutedEvent ComboBoxSelectionEvent2 =
            EventManager.RegisterRoutedEvent(nameof(ComboBoxSelection2), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(TextBoxGeneratorDataGrid));
        public event RoutedEventHandler ComboBoxSelection2
        {
            add => AddHandler(ComboBoxSelectionEvent2, value);
            remove => RemoveHandler(ComboBoxSelectionEvent2, value);
        }

        public class ViewModel : Bindable
        {
            private readonly TextBoxGeneratorDataGrid datagrid_;
            public bool CanAdd => datagrid_.Items.Count < datagrid_.MaxItemCount;

            public ViewModel(TextBoxGeneratorDataGrid _datagrid)
            {
                datagrid_ = _datagrid;
            }

            public void Refresh()
            {
                OnPropertyChanged(nameof(CanAdd));
            }
        }

        #region _UNUSED
        // 그냥 버튼에 이벤트 넣음. 이렇게 하니까 호출안되는 경우가 있는 것 같다.
        private void ButtonAddCellLoaded(object _sender, RoutedEventArgs _e)
        {
            var cell = _sender as DataGridCell;
            if (cell == null)
                return;

            var addButton = cell.FindChild<Button>();
            if (addButton == null)
                return;

            addButton.Click += (_1, _2) =>
            {
            };
        }
        private void ButtonMinusCellLoaded(object _sender, RoutedEventArgs _e)
        {
            var cell = _sender as DataGridCell;
            if (cell == null)
                return;

            var minusButton = cell.FindChild<Button>();
            if (minusButton == null)
                return;

            minusButton.Click += (_1, _2) =>
            {

            };
        }
        #endregion

        private void ClickAddItem(object _sender, RoutedEventArgs _e)
        {
            AddItem();
        }

        public InternalDataGridItem? AddItem()
        {
            if (!VM.CanAdd)
            {
                Debug.Assert(false, "더이상 못해");
                return null;
            }

            var newItem = new InternalDataGridItem(Items);
            Items.Add(newItem);
            Items.ForEach(_x => _x.Refresh());
            VM.Refresh();
            return newItem;
        }

        private void ClickRemoveItem(object _sender, RoutedEventArgs _e)
        {
            if (_sender is not Button btn)
                return;
                
            var item = btn.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            Items.Remove(item);
            Items.ForEach(_x => _x.Refresh());
            VM.Refresh();
        }

        private void AutoCompleteTextBox1_OnTextChanged(object _sender, RoutedEventArgs _e)
        {
            var tb = _sender as AutoCompleteTextBox;
            if (tb == null)
                return;

            var item = tb.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            var args = new InternalDataGridItemTextChangedEventArgs(TextChangedEvent1, item);
            RaiseEvent(args);
        }

        private void AutoCompleteTextBox2_OnTextChanged(object _sender, RoutedEventArgs _e)
        {
            var tb = _sender as AutoCompleteTextBox;
            if (tb == null)
                return;

            var item = tb.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            var args = new InternalDataGridItemTextChangedEventArgs(TextChangedEvent2, item);
            RaiseEvent(args);
        }

        private void ComboBox1_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            var tb = _sender as ComboBox;
            if (tb == null)
                return;

            var item = tb.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            var args = new InternalDataGridComboBoxSelectionChangedEventArgs(ComboBoxSelectionEvent1, item);
            RaiseEvent(args);
        }

        private void ComboBox2_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            var tb = _sender as ComboBox;
            if (tb == null)
                return;

            var item = tb.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            var args = new InternalDataGridComboBoxSelectionChangedEventArgs(ComboBoxSelectionEvent2, item);
            RaiseEvent(args);
        }

        private void DataGridCell_Loaded(object _sender, RoutedEventArgs _e)
        {
            var cell = _sender as DataGridCell;
            if (cell == null)
                return;
            var item = cell.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            int index = ItemsDataGrid.Columns.IndexOf(cell.Column);
            if (index == 0)
            {
                item.ComboBox1 = cell.FindChild<ComboBox>()!;
                item.ComboBoxCell1 = cell;
            }
            else if (index == 1)
            {
                item.TextBox1 = cell.FindChild<AutoCompleteTextBox>()!;
                item.TextBoxCell1 = cell;
            }
            else if (index == 2)
            {
                item.TextBox2 = cell.FindChild<AutoCompleteTextBox>()!;
                item.TextBoxCell2 = cell;
                
            }
        }

        private void DataGridRow_Loaded(object _sender, RoutedEventArgs _e)
        {
            var row = _sender as DataGridRow;
            if (row == null)
                return;
            var item = row.DataContext as InternalDataGridItem;
            if (item == null)
                return;

            // 탭 변경할 때마다 호출되서. 막음
            if (item.IsLoaded)
                return;

            item.IsLoaded = true;

            // 초기 콤보박스 SelectedChanged 이벤트 호출을 위해
            Dispatcher.BeginInvoke(() =>
            {
                //item.SetSelectedIndex(0);
                item.OnLoaded();
            });
        }

        public void Clear()
        {
            for (int i = Items.Count - 1; i >= 1; --i)
                Items.RemoveAt(i);

            First.Text1 = string.Empty;
            First.Text2 = string.Empty;
            First.ComboBox1.SelectedIndex = 0;
        }
    }
}
