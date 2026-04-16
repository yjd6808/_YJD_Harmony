/*
 * 작성자: 윤정도
 * 생성일: 3/13/2023 12:27:16 AM
 *
 */

using SGToolsCommon;
using SGToolsCommon.Sga;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class AlbumViewModel : Bindable
    {
        private SgaPackage selectedPackage_ = null!;
        private SgaImage selectedImage_ = null!;

        //////////////////////////////////////////////////////////////////////////////////
        public AlbumViewModel(AlbumView _view, MainViewModel _mainViewModel)
        {
            View = _view;
            MainViewModel = _mainViewModel;
        }

        public SgaPackage SelectedPackage
        {
            get => selectedPackage_;
            set
            {
                selectedPackage_ = value;
                OnPropertyChanged();
            }
        }

        public SgaImage SelectedImage
        {
            get => selectedImage_;
            set
            {
                selectedImage_ = value;
                OnPropertyChanged();
            }
        }

        public AlbumView View { get; }
        public MainViewModel MainViewModel { get; }
    }
}
