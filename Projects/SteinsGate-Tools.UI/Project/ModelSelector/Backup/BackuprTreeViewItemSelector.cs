/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 7:26:18 PM
 *
 */

using System;
using System.Windows;
using System.Windows.Controls;
using SGToolsCommon.Extension;
using SGToolsUI.Model.Backup;

namespace SGToolsUI.ModelSelector.Backup
{
    public class BackuprTreeViewItemSelector : DataTemplateSelector
    {
        public static readonly BackuprTreeViewItemSelector Selector = new();

        //////////////////////////////////////////////////////////////////////////////////
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            TreeView? treeView = container.FindParent<TreeView>();

            switch (item)
            {
                case BackupTreeViewItemGroup group: return (DataTemplate)Application.Current.FindResource("BackupTreeViewItemGroupTemplate");
                case BackupTreeViewItem backupItem: return (DataTemplate)Application.Current.FindResource("BackupTreeViewItemTemplate");
            }

            throw new Exception("BackuprTreeViewItemSelector 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
