// jdyun 24/10/04(토)
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

namespace SGToolsCommon.Extension
{
    public static class DataGridEx
    {
        // https://stackoverflow.com/questions/31404400/how-to-get-column-index-by-column-name
        public static DataGridCell? GetCell(this DataGrid _grid, DataGridRow _row, int _columnIndex = 0)
        {
            if (_row == null)
                return null;

            var presenter = _row.FindChild<DataGridCellsPresenter>();
            if (presenter == null)
                return null;

            var cell = presenter.ItemContainerGenerator.ContainerFromIndex(_columnIndex) as DataGridCell;
            if (cell != null)
                return cell;

            _grid.ScrollIntoView(_row, _grid.Columns[_columnIndex]);
            cell = (DataGridCell)presenter.ItemContainerGenerator.ContainerFromIndex(_columnIndex);
            return cell;
        }

        // 데이터그리드의 row에서 headerName 컬럼명에 해당하는 cell을 찾아서 비주얼 칠드런중 T에 해당하는 자식을 찾아서 반환한다.
        public static T? GetChildInCell<T>(this DataGrid _grid, DataGridRow _row, string _headerName) where T : FrameworkElement
        {
            DataGridColumn? column = _grid.Columns.FirstOrDefault(_x => (string)_x.Header == _headerName);
            if (column == null)
                return null;

            int index = _grid.Columns.IndexOf(column);
            if (index < 0)
                return null;

            DataGridCell? cell = _grid.GetCell(_row, index);
            if (cell == null)
                return null;

            T? frameworkElement = cell.FindChild<T>();
            if (frameworkElement == null)
                return null;
            return frameworkElement;
        }
    }
}
