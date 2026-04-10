/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 7:19:55 PM
 *
 * 백업 뷰에서 단순히 뼈대만 보여주는 용도
 */

using System;
using System.Collections.Generic;
using SGToolsUI.Model.Main;

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

        //////////////////////////////////////////////////////////////////////////////////
        public void ForEachRecursive(Action<BackupTreeViewItem> _action)
        {
            Children.ForEach(child =>
            {
                _action(child);

                if (child is BackupTreeViewItemGroup)
                    (child as BackupTreeViewItemGroup).ForEachRecursive(_action);
            });
        }
    }
}
