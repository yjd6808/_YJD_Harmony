/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 4:12:28 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
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
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public enum ClipboardOperate
    {
        Copy,
        Paste,
        Cut
    }

    public class ClipboardOperateUIElement : MainCommandAbstractAsync, INotifyPropertyChanged
    {
        private List<SGUIElement> _clipboard = new();
        public int ClipboardDataCount => _clipboard.Count;
        public bool HasClipboardData => _clipboard.Count > 0;

        public ClipboardOperateUIElement(MainViewModel viewModel) 
            : base(viewModel, "UI 엘리먼트 대상으로 클립보드 오퍼레이션을 수행합니다.")
        {
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            if (parameter is not ClipboardOperate operate)
                return;

            ObservableCollection<SGUIElement> selectedElements = ViewModel.GroupMaster.SelectedElements;
            int selectedElementCount = selectedElements.Count;

            switch (operate)
            {
                case ClipboardOperate.Copy:
                    
                    if (selectedElementCount > 0)
                        Copy(selectedElements);
                    break;
                case ClipboardOperate.Paste:
                    await Paste(selectedElements); 
                    break;
                case ClipboardOperate.Cut:
                    if (selectedElementCount > 0)
                        Cut(selectedElements);
                    break;
            }
        }

        private bool Copy(ObservableCollection<SGUIElement> selectedElements)
        {
            // 잘못된 붙여넣기를 할 우려가 있으므로 Depth가 틀리면 못하도록 막는다.
            int depth = selectedElements[0].Depth;
            if (selectedElements.FirstOrDefault(element => element.Depth != depth) != null)
            {
                MessageBoxEx.ShowTopMost("깊이가 다른 원소가 존재합니다.\n", "안돼!");
                return false;
            }

            _clipboard.Clear();
            for (int i = 0; i < selectedElements.Count; ++i)
                _clipboard.Add(selectedElements[i]);
            OnPropertyChanged(nameof(ClipboardDataCount));
            OnPropertyChanged(nameof(HasClipboardData));
            return true;
        }

        private void Cut(ObservableCollection<SGUIElement> selectedElements)
        {
            if (Copy(selectedElements))
            {
                // 삭제 수행시 알아서 백업하므로 여기서 백업 안해도됨.
                ViewModel.Commander.DeleteUIElement.Execute(null);
            }
        }

        private async Task Paste(ObservableCollection<SGUIElement> selectedElements)
        {
            if (_clipboard.Count <= 0)
            {
                MessageBoxEx.ShowTopMost("붙여넣을 데이터가 없습니다.");
                return;
            }

            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;
            SGUIGroup pasteGroup;
            SGUIGroup cloned = new SGUIGroup(_clipboard.Count);

            for (int i = 0; i < _clipboard.Count; ++i)
            {
                SGUIElement element = _clipboard[i].Clone() as SGUIElement;
                cloned.Children.Add(element);
            }

            if (groupMaster.HasSelectedElement)
            {
                SGUIElement standardElement = selectedElements[selectedElements.Count - 1];

                // 그룹이 아닌 경우 무조건 엘리먼트 이후로 추가해줌
                if (!standardElement.IsGroup)
                {
                    await InsertChildren(standardElement.Parent, cloned, standardElement.Index + 1);
                    return;
                }

                SGUIGroup standardGroup = standardElement.Cast<SGUIGroup>();

                // 선택한 그룹이 확장되어 있으면 안에 넣고
                // 확장되어있지 않으면 해당 그룹이 아닌것처럼 추가해준다.
                if (standardGroup.ItemLoaded && standardGroup.Item.IsExpanded)
                    await InsertChildren(standardGroup, cloned, 0);
                else
                    await InsertChildren(standardGroup.Parent, cloned, standardGroup.Index + 1);
            }
            else
            {
                await InsertChildren(groupMaster, cloned, groupMaster.SelectedElements.Count);
            }
        }

        private async Task InsertChildren(SGUIGroup group, SGUIGroup cloned, int index)
        {
            if (group.IsMaster && !cloned.HasOnlyGroup)
            {
                MessageBox.Show("그룹마스터에는 그룹만 붙일 수 있습니다.");
                return;
            }

            group.InsertChildren(cloned, index);

            // 새로 추가된 원소들 선택함
            ViewModel.GroupMaster.DeselectAll();
            cloned.ForEachRecursive(element => element.Selected = true);
            await ViewModel.Saver.BackupAsync("붙여넣기");
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public void Clear()
        {
            _clipboard.Clear();

            OnPropertyChanged(nameof(HasClipboardData));
            OnPropertyChanged(nameof(ClipboardDataCount));
            
        }
    }
}
