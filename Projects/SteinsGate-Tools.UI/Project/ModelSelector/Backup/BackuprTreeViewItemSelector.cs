/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 7:26:18 PM
 *
 */

using SGToolsCommon.Extension;
using SGToolsUI.CustomControl;
using SGToolsUI.Model.Main;
using SGToolsUI.ModelSelector.Main;
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
using SGToolsUI.Model.Backup;

namespace SGToolsUI.ModelSelector.Backup
{
    public class BackuprTreeViewItemSelector : DataTemplateSelector
    {
        public static readonly BackuprTreeViewItemSelector Selector = new();

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            TreeView treeView = container.FindParent<TreeView>();

            switch (item)
            {
                case BackupTreeViewItemGroup __group__: return (DataTemplate)Application.Current.FindResource("BackupTreeViewItemGroupTemplate");
                case BackupTreeViewItem __else__: return (DataTemplate)Application.Current.FindResource("BackupTreeViewItemTemplate");
            }

            throw new Exception("BackuprTreeViewItemSelector 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
