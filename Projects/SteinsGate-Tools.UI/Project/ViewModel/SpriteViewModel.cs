/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:42:59 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Drawing;
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
    public class SpriteViewModel : Bindable
    {
        public SpriteViewModel()
        {
            PreviewTarget = new SgaSprite();
        }

        public SpriteView View { get; set; }       // Xaml에서 주입

        public SgaSpriteAbstract PreviewTarget
        {
            get => _previewTarget;
            set
            {
                _previewTarget = value;

                if (_previewTarget.IsLink)
                {
                    SgaSprite link = _previewTarget.Parent.SpriteList[_previewTarget.TargetFrameIndex] as SgaSprite;

                    if (link == null)
                        throw new Exception("링크 스프라이트가 가리키는 스프라이트가 SgaSprite이 아닙니다.");

                    RealTarget = link;
                }
                else
                {
                    SgaSprite preview = _previewTarget as SgaSprite;

                    if (preview == null)
                        throw new Exception("링크 스프라이트가 아닌데 SgaSprite 타입이 아닙니다.");

                    RealTarget = preview;
                }

                OnPropertyChanged();
            }
        }

        public SgaSprite RealTarget
        {
            get => _realTarget;
            set
            {
                _realTarget = value;
                BitmapSource = _realTarget.Source;
                OnPropertyChanged();
            }
        }
        public BitmapSource BitmapSource
        {
            get => _bitmapSource;
            set
            {
                _bitmapSource = value;
                OnPropertyChanged();
            }
        }

        private SgaSpriteAbstract _previewTarget;
        private SgaSprite _realTarget;
        private BitmapSource _bitmapSource;
    }
}
