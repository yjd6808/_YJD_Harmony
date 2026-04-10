/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:42:59 AM
 *
 */

using System;
using System.Windows.Media.Imaging;
using SGToolsCommon;
using SGToolsCommon.Sga;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class SpriteViewModel : Bindable
    {
        private SgaSpriteAbstract previewTarget_;
        private SgaSprite realTarget_;
        private BitmapSource bitmapSource_;

        //////////////////////////////////////////////////////////////////////////////////
        public SpriteViewModel()
        {
            PreviewTarget = new SgaSprite();
        }

        public SpriteView View { get; set; }       // Xaml에서 주입

        public SgaSpriteAbstract PreviewTarget
        {
            get => previewTarget_;
            set
            {
                previewTarget_ = value;

                if (previewTarget_.IsLink)
                {
                    SgaSprite link = previewTarget_.Parent.SpriteList[previewTarget_.TargetFrameIndex] as SgaSprite;

                    if (link == null)
                        throw new Exception("링크 스프라이트가 가리키는 스프라이트가 SgaSprite이 아닙니다.");

                    RealTarget = link;
                }
                else
                {
                    SgaSprite preview = previewTarget_ as SgaSprite;

                    if (preview == null)
                        throw new Exception("링크 스프라이트가 아닌데 SgaSprite 타입이 아닙니다.");

                    RealTarget = preview;
                }

                OnPropertyChanged();
            }
        }

        public SgaSprite RealTarget
        {
            get => realTarget_;
            set
            {
                realTarget_ = value;
                BitmapSource = realTarget_.Source;
                OnPropertyChanged();
            }
        }

        public BitmapSource BitmapSource
        {
            get => bitmapSource_;
            set
            {
                bitmapSource_ = value;
                OnPropertyChanged();
            }
        }
    }
}
