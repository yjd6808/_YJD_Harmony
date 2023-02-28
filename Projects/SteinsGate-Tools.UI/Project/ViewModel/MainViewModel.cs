/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:33:20 AM
 *
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
using SGToolsCommon;
using SGToolsCommon.Sga;
using SGToolsUI.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class MainViewModel : Bindable
    {
        public MainViewModel()
        {
            Setting = new Setting();
            Setting.Load();
            PackManager = SgaManager.Instance;
            Commander = new MainCommandCenter(this);
            Commander.Execute(nameof(ReloadPackage));
        }

        public MainView View { get; set; }
        public Setting Setting { get; set; }
        public MainCommandCenter Commander { get; }
        public SgaManager PackManager { get; }

        public SgaPackage SelectedPackage
        {
            get => _selectedPackage;
            set
            {
                _selectedPackage = value;
                OnPropertyChanged();
            }
        }

        public SgaImage SelectedImage
        {
            get => _selectedImage;
            set
            {
                _selectedImage = value;
                OnPropertyChanged();
            }
        }

      

        private SgaPackage _selectedPackage = new ();
        private SgaImage _selectedImage = new ();
    }
}
