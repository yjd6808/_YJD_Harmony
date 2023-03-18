/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 4:58:29 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
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
using Microsoft.Xaml.Behaviors.Core;
using SGToolsCommon;
using SGToolsCommon.Extension;
using Vanara.PInvoke;

namespace SGToolsUI.ViewModel
{
    

    public class BackupViewModel : Bindable
    {
       

        public BackupViewModel()
        {
            _items = new List<BackupTreeViewItem>();
            _backupDates = Directory.GetFiles(Constant.BackupDirectoryRoot, "*", SearchOption.TopDirectoryOnly).ToList();
            _backupDates = _backupDates.Where(folder => DirectoryEx.IsDirectory(folder)).ToList();
            _backupDates.Sort((s1, s2) => s1.CompareTo(s2) * -1);   // 오름차순 정렬
        }

        public List<string> BackUpDates
        {
            get => _backupDates;
            set
            {
                _backupDates = value;
                OnPropertyChanged();
            }
        }

        public List<BackupTreeViewItem> Items
        {
            get => _items;
            set
            {
                _items = value;
                OnPropertyChanged();
            }
        }


        private List<BackupTreeViewItem> _items;
        private List<string> _backupDates;
    }


    public class BackupTreeViewItem
    {

    }
}


