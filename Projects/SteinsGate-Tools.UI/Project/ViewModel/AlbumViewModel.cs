/*
 * 작성자: 윤정도
 * 생성일: 3/13/2023 12:27:16 AM
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
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class AlbumViewModel : Bindable
    {
        public AlbumViewModel(AlbumView view, MainViewModel mainViewModel)
        {
            View = view;
            MainViewModel = mainViewModel;
        }

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

        public AlbumView View { get; }
        public MainViewModel  MainViewModel { get; }


        private SgaPackage _selectedPackage;
        private SgaImage _selectedImage;
    }
}
