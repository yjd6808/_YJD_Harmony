/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 7:19:55 PM
 *
 * 백업 뷰에서 단순히 뼈대만 보여주는 용도
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
using SGToolsUI.Model.Main;
using Vanara.PInvoke;

namespace SGToolsUI.Model.Backup
{
    public class BackupTreeViewItem
    {
        public string VisualName { get; set; } = string.Empty;
        public SGUIElementType UIElementType { get; set; }
    }

    public class BackupTreeViewItemGroup : BackupTreeViewItem
    {
        public List<BackupTreeViewItem> Children { get; set; } = new();

        public void ForEachRecursive(Action<BackupTreeViewItem> action)
        {
            Children.ForEach(child =>
            {
                action(child);

                if (child is BackupTreeViewItemGroup)
                    (child as BackupTreeViewItemGroup).ForEachRecursive(action);
            });
        }
    }
    
}
